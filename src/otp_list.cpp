#include <crypto.hpp>
#include <otp_list.hpp>

#include <yaml-cpp/yaml.h>

#include <cstdlib>
#include <fstream>

namespace otpd
{

ParserException::~ParserException() = default;

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

void OTPListSingleton::load() try
{
    std::vector<byte_t> ciphertext = read_file_into_buf(m_settings_path.string());
    std::string plaintext = decrypt_data(ciphertext, m_password);

    std::vector<TOTP> entries_temp;
    YAML::Node settings = YAML::Load(plaintext);
    for (std::size_t i = 0; i < settings.size(); ++i)
    {
        YAML::Node element = settings[i];
        entries_temp.emplace_back(element["issuer"].as<std::string>(),
                                  element["label"].as<std::string>(),
                                  element["secret_base32"].as<std::string>(),
                                  element["period"].as<unsigned>());
    }
    m_entries.swap(entries_temp);
    zero_data(plaintext);
}
catch (const YAML::Exception& e)
{
    throw ParserException(e.what());
}

void OTPListSingleton::dump() const try
{
    YAML::Node settings;
    for (const TOTP& entry : m_entries)
    {
        YAML::Node item;
        item["issuer"] = entry.issuer();
        item["label"] = entry.label();
        item["secret_base32"] = entry.secret_base32();
        item["period"] = entry.period();
        settings.push_back(item);
    }
    std::string plaintext = YAML::Dump(settings);
    std::vector<byte_t> ciphertext = encrypt_data(plaintext, m_password);
    zero_data(plaintext);
    write_buf_to_file(m_settings_path.string(), ciphertext);
}
catch (const YAML::Exception& e)
{
    throw ParserException(e.what());
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
