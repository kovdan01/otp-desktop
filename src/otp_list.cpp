#include <crypto.hpp>
#include <otp_list.hpp>
#include <utils.hpp>

#include <cstdlib>
#include <cstring>
#include <fstream>

namespace otpd
{

OTPListSingleton& OTPListSingleton::get_instance()
{
    static OTPListSingleton instance;
    return instance;
}

OTPListSingleton::OTPListSingleton()
{
    const char* home_dir_env = std::getenv("HOME");
    if (home_dir_env == nullptr)
        throw std::runtime_error("No environment variable HOME found!");
    std::filesystem::path home_dir(home_dir_env);

    m_app_dir = home_dir / ".otp-desktop";
    if (!std::filesystem::is_directory(m_app_dir))
        std::filesystem::create_directory(m_app_dir);

    m_settings_path = m_app_dir / "settings";
}

static std::vector<byte_t> read_file_into_buf(std::string_view filepath)
{
    std::ifstream f(filepath.data(), std::ios_base::binary);
    std::vector<byte_t> contents{std::istreambuf_iterator<char>{f}, std::istreambuf_iterator<char>{}};
    return contents;
}

static void write_buf_to_file(std::string_view filepath, std::span<const byte_t> data)
{
    std::ofstream f(filepath.data(), std::ios_base::binary);
    f.write(reinterpret_cast<const char*>(data.data()), data.size());
}

template <typename T>
static void serialize_number(T value, byte_t** where)
{
    std::memcpy(*where, &value, sizeof(value));
    *where += sizeof(value);
}

static void serialize_string(std::string_view value, byte_t** where)
{
    serialize_number<std::size_t>(value.size(), where);
    std::memcpy(*where, value.data(), value.size());
    *where += value.size();
}

template <typename T>
[[nodiscard]] static T deserialize_number(const byte_t** where, const byte_t* end)
{
    T value;
    if (static_cast<std::size_t>(end - *where) < sizeof(value))
        throw ParserException("Deserialization error: buffer size is too small");
    std::memcpy(&value, *where, sizeof(value));
    *where += sizeof(value);
    return value;
}

[[nodiscard]] static std::string deserialize_string(const byte_t** where, const byte_t* end)
{
    std::size_t size = deserialize_number<std::size_t>(where, end);
    if (static_cast<std::size_t>(end - *where) < size)
        throw ParserException("Deserialization error: buffer size is too small");
    std::string s;
    s.resize(size);
    std::memcpy(s.data(), *where, size);
    *where += size;
    return s;
}

void OTPListSingleton::load()
{
    std::vector<byte_t> ciphertext = read_file_into_buf(m_settings_path.string());
    std::vector<byte_t> plaintext = decrypt_data(ciphertext, m_password);

    const byte_t* where = plaintext.data();
    const byte_t* end = plaintext.data() + plaintext.size();
    std::size_t entries_count = deserialize_number<std::size_t>(&where, end);
    m_entries.reserve(entries_count);
    for (std::size_t i = 0; i < entries_count; ++i)
    {
        std::string issuer = deserialize_string(&where, end);
        std::string label = deserialize_string(&where, end);
        std::string secret_base32 = deserialize_string(&where, end);
        unsigned period = deserialize_number<unsigned>(&where, end);
        m_entries.emplace_back(std::move(issuer), std::move(label), std::move(secret_base32), period);
    }

    zero_data(plaintext);
}

void OTPListSingleton::dump() const
{
    std::size_t total_size = 0;
    total_size += sizeof(std::size_t);
    for (const TOTP& entry : m_entries)
    {
        total_size += entry.issuer().size() + sizeof(std::size_t);
        total_size += entry.label().size() + sizeof(std::size_t);
        total_size += entry.secret_base32().size() + sizeof(std::size_t);
        total_size += sizeof(unsigned);
    }
    std::vector<byte_t> buf(total_size);
    byte_t* where = buf.data();

    serialize_number<std::size_t>(m_entries.size(), &where);
    for (const TOTP& entry : m_entries)
    {
        serialize_string(entry.issuer(), &where);
        serialize_string(entry.label(), &where);
        serialize_string(entry.secret_base32(), &where);
        serialize_number<unsigned>(entry.period(), &where);
    }

    std::vector<byte_t> ciphertext = encrypt_data(buf, m_password);
    zero_data(buf);
    write_buf_to_file(m_settings_path.string(), ciphertext);
}

void OTPListSingleton::set_password(std::string password)
{
    zero_data(m_password);
    m_password = std::move(password);
}

OTPListSingleton::~OTPListSingleton()
{
    zero_data(m_password);
}

}  // namespace otpd
