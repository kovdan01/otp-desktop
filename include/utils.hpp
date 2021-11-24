#ifndef OTP_DESKTOP_UTILS_HPP_
#define OTP_DESKTOP_UTILS_HPP_

#include <QMessageBox>

#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>

namespace otpd
{

using byte_t = std::uint8_t;

class Exception : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
    ~Exception() override;
};

class CryptoException : public Exception
{
public:
    using Exception::Exception;
    ~CryptoException() override;
};

class TOTPException : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
    ~TOTPException() override;
};

class ParserException : public Exception
{
public:
    using Exception::Exception;
    ~ParserException() override;
};

void zero_data(std::span<byte_t> data) noexcept;
void zero_data(std::string& data) noexcept;

#define OTP_DESKTOP_CATCH_CRYPTO_EXCEPTION                                                                      \
    catch (const CryptoException& e)                                                                            \
    {                                                                                                           \
        QMessageBox::critical(this, tr("Oops!"),                                                                \
                              QString::fromStdString(std::string("Crypto error: ") + e.what()));                \
    }                                                                                                           \

#define OTP_DESKTOP_CATCH_TOTP_EXCEPTION                                                                        \
    catch (const TOTPException& e)                                                                              \
    {                                                                                                           \
        QMessageBox::critical(this, tr("Oops!"),                                                                \
                              QString::fromStdString(std::string("TOTP error: ") + e.what()));                  \
    }                                                                                                           \


#define OTP_DESKTOP_CATCH_PARSER_EXCEPTION                                                                      \
    catch (const ParserException& e)                                                                            \
    {                                                                                                           \
        QMessageBox::critical(this, tr("Oops!"),                                                                \
                              QString::fromStdString(std::string("Settings file parse error: ") + e.what()));   \
    }                                                                                                           \

#define OTP_DESKTOP_CATCH_STD_EXCEPTION                                                                         \
    catch (const std::exception& e)                                                                             \
    {                                                                                                           \
        QMessageBox::critical(this, tr("Oops!"),                                                                \
                              QString::fromStdString(std::string("Error: ") + e.what()));                       \
    }                                                                                                           \

#define OTP_DESKTOP_CATCH_ANY_EXCEPTION                                                                         \
    catch (...)                                                                                                 \
    {                                                                                                           \
        QMessageBox::critical(this, tr("Oops!"), tr("Unknown error"));                                          \
    }                                                                                                           \

}  // namespace otpd

#endif  // OTP_DESKTOP_UTILS_HPP_
