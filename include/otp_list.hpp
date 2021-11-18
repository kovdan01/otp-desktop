#ifndef OTP_DESKTOP_OTP_LIST_HPP_
#define OTP_DESKTOP_OTP_LIST_HPP_

#include <totp.hpp>

#include <filesystem>
#include <stdexcept>
#include <vector>

namespace otpd
{

class ParserException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
    ~ParserException() override;
};

class OTPListSingleton
{
public:
    static OTPListSingleton& get_instance();

    OTPListSingleton(const OTPListSingleton&) = delete;
    OTPListSingleton& operator=(const OTPListSingleton&) = delete;
    OTPListSingleton(OTPListSingleton&&) = delete;
    OTPListSingleton& operator=(OTPListSingleton&&) = delete;

    std::filesystem::path settings_path() const
    {
        return m_settings_path;
    }

    const std::vector<TOTP>& entries() const
    {
        return m_entries;
    }

    std::vector<TOTP>& entries()
    {
        return m_entries;
    }

    void load();
    void dump() const;

private:
    OTPListSingleton();
    ~OTPListSingleton();

    std::vector<TOTP> m_entries;
    std::filesystem::path m_app_dir;
    std::filesystem::path m_settings_path;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_OTP_LIST_HPP_
