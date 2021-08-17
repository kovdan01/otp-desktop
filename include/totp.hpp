#ifndef OTP_DESKTOP_TOTP_HPP_
#define OTP_DESKTOP_TOTP_HPP_

#include <string>
#include <utility>
#include <vector>

namespace otpd
{

class TOTPSingleton
{
public:
    static TOTPSingleton& get_instance();

    TOTPSingleton(const TOTPSingleton& root) = delete;
    TOTPSingleton& operator=(const TOTPSingleton&) = delete;
    TOTPSingleton(TOTPSingleton&& root) = delete;
    TOTPSingleton& operator=(TOTPSingleton&&) = delete;

private:
    TOTPSingleton();
    ~TOTPSingleton();
};

class TOTP
{
public:
    using byte_t = char;

    enum class Algorithm
    {
        SHA1,
        SHA256,
        SHA512,
    };

    TOTP(std::string issuer, std::string label, std::string secret_base32);

    TOTP(const TOTP&) = default;
    TOTP& operator=(const TOTP&) = default;
    TOTP(TOTP&&) = default;
    TOTP& operator=(TOTP&&) = default;

    std::string generate() const;

private:
    std::string m_issuer;
    std::string m_label;
    std::string m_secret_base32;
    std::vector<byte_t> m_secret;
    unsigned m_period = 30;
    unsigned m_digits = 6;
    Algorithm m_algorithm = Algorithm::SHA1;
};

}  // namespace otpd

#endif  // OTP_DESKTOP_TOTP_HPP_
