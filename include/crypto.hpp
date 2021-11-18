#ifndef OTP_DESKTOP_CRYPTO_HPP_
#define OTP_DESKTOP_CRYPTO_HPP_

#include <gcrypt.h>

#include <cstdint>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace otpd
{

using byte_t = std::uint8_t;

class GcryCipherWrapper
{
public:
    GcryCipherWrapper(int algo, int mode, unsigned int flags);
    ~GcryCipherWrapper();

    void setkey(std::span<const byte_t> key);
    void setiv(std::span<const byte_t> init_vector);
    void encrypt(std::span<byte_t> out, std::span<const byte_t> in);
    void decrypt(std::span<byte_t> out, std::span<const byte_t> in);

    [[nodiscard]] const gcry_cipher_hd_t raw() const noexcept
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

    [[nodiscard]] const gcry_mac_hd_t raw() const noexcept
    {
        return m_raw;
    }

private:
    gcry_mac_hd_t m_raw = nullptr;
};

GcryCipherWrapper init_cipher(std::span<byte_t> key, std::span<byte_t> init_vector);

std::vector<byte_t> encrypt_data(std::string_view plaintext, std::string_view password);
std::string decrypt_data(std::span<const byte_t> data, std::string_view password);

}  // namespace otpd

#endif  // OTP_DESKTOP_CRYPTO_HPP_
