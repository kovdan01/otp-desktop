#include <otp_list.hpp>

#include <yaml-cpp/yaml.h>

#include <cstdlib>

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

    m_settings_path = m_app_dir / "settings.yaml";

    YAML::Node settings = YAML::LoadFile(m_settings_path.string());
    for (std::size_t i = 0; i < settings.size(); ++i)
    {
        YAML::Node element = settings[i];
        m_entries.emplace_back(element["issuer"].as<std::string>(),
                               element["label"].as<std::string>(),
                               element["secret_base32"].as<std::string>());
    }
}

OTPListSingleton::~OTPListSingleton() = default;

}  // namespace otpd
