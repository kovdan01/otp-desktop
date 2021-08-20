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

    m_settings_path = m_app_dir / "settings.yaml";
}

void OTPListSingleton::load() try
{
    std::vector<TOTP> entries_temp;
    YAML::Node settings = YAML::LoadFile(m_settings_path.string());
    for (std::size_t i = 0; i < settings.size(); ++i)
    {
        YAML::Node element = settings[i];
        entries_temp.emplace_back(element["issuer"].as<std::string>(),
                                  element["label"].as<std::string>(),
                                  element["secret_base32"].as<std::string>(),
                                  element["period"].as<unsigned>());
    }
    m_entries.swap(entries_temp);
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
    std::ofstream(m_settings_path) << YAML::Dump(settings);
}
catch (const YAML::Exception& e)
{
    throw ParserException(e.what());
}

OTPListSingleton::~OTPListSingleton() = default;

}  // namespace otpd
