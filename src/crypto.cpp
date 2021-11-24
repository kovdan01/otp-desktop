#include <crypto.hpp>
#include <utils.hpp>

#include <gcrypt.h>

#include <cstdint>
#include <cstring>
#include <sstream>

namespace otpd
{

static constexpr std::size_t AES256_KEY_SIZE = 32;
static constexpr std::size_t AES256_BLOCK_SIZE = 16;
static constexpr std::size_t HMAC_KEY_SIZE = 64;
static constexpr std::size_t KDF_ITERATIONS = 100'000;
static constexpr std::size_t KDF_SALT_SIZE = 128;
static constexpr std::size_t KDF_KEY_SIZE = AES256_KEY_SIZE + HMAC_KEY_SIZE;

class GcryCipherWrapper
{
public:
    GcryCipherWrapper(int algo, int mode, unsigned int flags);
    ~GcryCipherWrapper();

    void setkey(std::span<const byte_t> key);
    void setiv(std::span<const byte_t> init_vector);
    void encrypt(std::span<byte_t> out, std::span<const byte_t> in);
    void decrypt(std::span<byte_t> out, std::span<const byte_t> in);

    [[nodiscard]] gcry_cipher_hd_t raw() const noexcept
    {
        return m_raw;
    }

private:
    gcry_cipher_hd_t m_raw = nullptr;
};

class GcryMacWrapper
{
public:
    GcryMacWrapper(int algo, unsigned int flags, gcry_ctx_t ctx);
    ~GcryMacWrapper();

    void setkey(std::span<const byte_t> key);
    void write(std::span<const byte_t> data);
    std::size_t read(std::span<byte_t> buffer);
    void verify(std::span<const byte_t> hmac);

    [[nodiscard]] gcry_mac_hd_t raw() const noexcept
    {
        return m_raw;
    }

private:
    gcry_mac_hd_t m_raw = nullptr;
};

GcryCipherWrapper::GcryCipherWrapper(int algo, int mode, unsigned int flags)
{
    gcry_error_t err = gcry_cipher_open(&m_raw, algo, mode, flags);
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "cipher_open: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

GcryCipherWrapper::~GcryCipherWrapper()
{
    if (m_raw != nullptr)
    {
        gcry_cipher_close(m_raw);
    }
}

void GcryCipherWrapper::setkey(std::span<const byte_t> key)
{
    gcry_error_t err = gcry_cipher_setkey(m_raw, key.data(), key.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "cipher_setkey: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

void GcryCipherWrapper::setiv(std::span<const byte_t> init_vector)
{
    gcry_error_t err = gcry_cipher_setiv(m_raw, init_vector.data(), init_vector.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "cipher_setiv: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

void GcryCipherWrapper::encrypt(std::span<byte_t> out, std::span<const byte_t> in)
{
    gcry_error_t err = gcry_cipher_encrypt(m_raw, out.data(), out.size(), in.data(), in.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "cipher_encrypt: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

void GcryCipherWrapper::decrypt(std::span<byte_t> out, std::span<const byte_t> in)
{
    gcry_error_t err = gcry_cipher_decrypt(m_raw, out.data(), out.size(), in.data(), in.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "cipher_decrypt: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

GcryMacWrapper::GcryMacWrapper(int algo, unsigned int flags, gcry_ctx_t ctx)
{
    gcry_error_t err = gcry_mac_open(&m_raw, algo, flags, ctx);
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "mac_open: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

GcryMacWrapper::~GcryMacWrapper()
{
    if (m_raw != nullptr)
    {
        gcry_mac_close(m_raw);
    }
}

void GcryMacWrapper::setkey(std::span<const byte_t> key)
{
    gcry_error_t err = gcry_mac_setkey(m_raw, key.data(), key.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "mac_setkey: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

void GcryMacWrapper::write(std::span<const byte_t> data)
{
    gcry_error_t err = gcry_mac_write(m_raw, data.data(), data.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "mac_write: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

std::size_t GcryMacWrapper::read(std::span<byte_t> buffer)
{
    std::size_t len;
    gcry_error_t err = gcry_mac_read(m_raw, buffer.data(), &len);
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "mac_read during encryption: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
    return len;
}

void GcryMacWrapper::verify(std::span<const byte_t> hmac)
{
    gcry_error_t err = gcry_mac_verify(m_raw, hmac.data(), hmac.size());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "HMAC verification failed: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }
}

static GcryCipherWrapper init_cipher(std::span<byte_t> key, std::span<byte_t> init_vector)
{
    // 256-bit AES using cipher-block chaining; with ciphertext stealing, no manual padding is required
    GcryCipherWrapper cipher(GCRY_CIPHER_AES256,
                             GCRY_CIPHER_MODE_CBC,
                             GCRY_CIPHER_CBC_CTS);

    cipher.setkey(key);
    cipher.setiv(init_vector);

    return cipher;
}

std::vector<byte_t> encrypt_data(std::span<const byte_t> data, std::string_view password)
{
    std::array<byte_t, AES256_BLOCK_SIZE> init_vector;
    std::array<byte_t, KDF_SALT_SIZE> kdf_salt;
    std::array<byte_t, KDF_KEY_SIZE> kdf_key;
    std::array<byte_t, AES256_KEY_SIZE> aes_key;
    std::array<byte_t, HMAC_KEY_SIZE> hmac_key;
    gcry_error_t err;

    std::uint8_t padding = AES256_BLOCK_SIZE - (data.size() % AES256_BLOCK_SIZE);
    std::vector<byte_t> plaintext(data.size() + padding);
    std::memcpy(plaintext.data(), data.data(), data.size());
    std::memset(plaintext.data() + data.size(), padding, padding);

    // Generate 128 byte salt in preparation for key derivation
    gcry_create_nonce(kdf_salt.data(), kdf_salt.size());

    // Key derivation: PBKDF2 using SHA512 w/ 128 byte salt over 10 iterations into a 64 byte key
    err = gcry_kdf_derive(password.data(),
                          password.size(),
                          GCRY_KDF_PBKDF2,
                          GCRY_MD_SHA512,
                          kdf_salt.data(),
                          kdf_salt.size(),
                          KDF_ITERATIONS,
                          kdf_key.size(),
                          kdf_key.data());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "kdf_derive: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }

    // Copy the first 32 bytes of kdf_key into aes_key
    std::memcpy(aes_key.data(), kdf_key.data(), aes_key.size());

    // Copy the last 32 bytes of kdf_key into hmac_key
    std::memcpy(hmac_key.data(), kdf_key.data() + aes_key.size(), hmac_key.size());

    // Generate the initialization vector
    gcry_create_nonce(init_vector.data(), init_vector.size());

    // Begin encryption
    GcryCipherWrapper cipher = init_cipher(aes_key, init_vector);

    // Encryption is performed in-place
    cipher.encrypt(plaintext, {});

    // Compute and allocate space required for packed data
    std::vector<byte_t> hmac(gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA512));
    std::vector<byte_t> packed_data(kdf_salt.size() + init_vector.size() + plaintext.size() + hmac.size());

    // Pack data before writing: salt::IV::ciphertext::HMAC where "::" denotes concatenation
    std::memcpy(packed_data.data(), kdf_salt.data(), kdf_salt.size());
    std::memcpy(packed_data.data() + kdf_salt.size(), init_vector.data(), init_vector.size());
    std::memcpy(packed_data.data() + kdf_salt.size() + init_vector.size(), plaintext.data(), plaintext.size());

    // Begin HMAC computation on encrypted/packed data
    GcryMacWrapper mac(GCRY_MAC_HMAC_SHA512, 0, nullptr);
    mac.setkey(hmac_key);

    // Add packed_data to the MAC computation
    mac.write({packed_data.data(), packed_data.size() - hmac.size()});

    // Finalize MAC and save it in the hmac buffer
    std::size_t hmac_len = mac.read(hmac);

    // Append the computed HMAC to packed_data
    std::memcpy(packed_data.data() + kdf_salt.size() + init_vector.size() + plaintext.size(), hmac.data(), hmac_len);

    zero_data(plaintext);
    zero_data(kdf_key);
    zero_data(aes_key);
    zero_data(hmac_key);

    return packed_data;
}

std::vector<byte_t> decrypt_data(std::span<const byte_t> data, std::string_view password)
{
    std::array<byte_t, AES256_BLOCK_SIZE> init_vector;
    std::array<byte_t, KDF_SALT_SIZE> kdf_salt;
    std::array<byte_t, KDF_KEY_SIZE> kdf_key;
    std::array<byte_t, AES256_KEY_SIZE> aes_key;
    std::array<byte_t, HMAC_KEY_SIZE> hmac_key;

    gcry_error_t err;

    // Compute necessary lengths
    std::vector<byte_t> hmac(gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA512));
    std::vector<byte_t> ciphertext;
    ciphertext.resize(data.size() - kdf_salt.size() - init_vector.size() - hmac.size());

    // Unpack data
    std::memcpy(kdf_salt.data(), data.data(), kdf_salt.size());
    std::memcpy(init_vector.data(), data.data() + kdf_salt.size(), init_vector.size());
    std::memcpy(ciphertext.data(), data.data() + kdf_salt.size() + init_vector.size(), ciphertext.size());
    std::memcpy(hmac.data(), data.data()  + kdf_salt.size() + init_vector.size() + ciphertext.size(), hmac.size());

    // Key derivation: PBKDF2 using SHA512 w/ 128 byte salt over 10 iterations into a 64 byte key
    err = gcry_kdf_derive(password.data(),
                          password.size(),
                          GCRY_KDF_PBKDF2,
                          GCRY_MD_SHA512,
                          kdf_salt.data(),
                          kdf_salt.size(),
                          KDF_ITERATIONS,
                          kdf_key.size(),
                          kdf_key.data());
    if (err != 0)
    {
        std::ostringstream ss;
        ss << "kdf_derive: " << gcry_strsource(err) << '/' << gcry_strerror(err);
        throw CryptoException(ss.str());
    }

    // Copy the first 32 bytes of kdf_key into aes_key
    std::memcpy(aes_key.data(), kdf_key.data(), aes_key.size());

    // Copy the last 32 bytes of kdf_key into hmac_key
    std::memcpy(hmac_key.data(), kdf_key.data() + aes_key.size(), hmac_key.size());

    // Begin HMAC verification
    GcryMacWrapper mac(GCRY_MAC_HMAC_SHA512, 0, nullptr);
    mac.setkey(hmac_key);
    mac.write({data.data(), kdf_salt.size() + init_vector.size() + ciphertext.size()});

    // Verify HMAC
    mac.verify(hmac);

    // Begin decryption
    GcryCipherWrapper cipher = init_cipher(aes_key, init_vector);
    cipher.decrypt(std::span<byte_t>{reinterpret_cast<byte_t*>(ciphertext.data()), ciphertext.size()}, {});

    // Delete padding
    ciphertext.resize(ciphertext.size() - ciphertext.back());

    zero_data(kdf_key);
    zero_data(aes_key);
    zero_data(hmac_key);

    return ciphertext;
}

}  // namespace otpd
