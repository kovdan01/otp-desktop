#ifndef OTP_DESKTOP_OTP_LIST_HPP_
#define OTP_DESKTOP_OTP_LIST_HPP_

#include <totp.hpp>

#include <filesystem>
#include <vector>

namespace otpd
{

class OTPListSingleton
{
public:
    static OTPListSingleton& get_instance();

    OTPListSingleton(const OTPListSingleton&) = delete;
    OTPListSingleton& operator=(const OTPListSingleton&) = delete;
    OTPListSingleton(OTPListSingleton&&) = delete;
    OTPListSingleton& operator=(OTPListSingleton&&) = delete;

    const std::vector<TOTP>& entries() const
    {
        return m_entries;
    }

    std::vector<TOTP>& entries()
    {
        return m_entries;
    }

private:
    OTPListSingleton();
    ~OTPListSingleton();

    std::vector<TOTP> m_entries;
    std::filesystem::path m_app_dir;
    std::filesystem::path m_settings_path;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_OTP_LIST_HPP_
