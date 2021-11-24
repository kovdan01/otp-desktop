#include <totp.hpp>
#include <utils.hpp>

#include <oath.h>

#include <cassert>
#include <chrono>
#include <cstring>
#include <ctime>
#include <stdexcept>

namespace otpd
{

static void check_oath_result(int res)
{
    if (res != 0)
        throw TOTPException("OATH error: \"" + std::string(oath_strerror(res)) + "\"");
}

TOTPSingleton& TOTPSingleton::get_instance()
{
    static TOTPSingleton instance;
    return instance;
}

TOTPSingleton::TOTPSingleton()
{
    int res = oath_init();
    check_oath_result(res);
}

TOTPSingleton::~TOTPSingleton()
{
    int res = oath_done();
    try
    {
        check_oath_result(res);
    }
    catch (const TOTPException& e)
    {
        assert(false);
    }
}

TOTP::TOTP(std::string issuer, std::string label, std::string secret_base32, unsigned period)
    : m_issuer(std::move(issuer))
    , m_label(std::move(label))
    , m_secret_base32(std::move(secret_base32))
    , m_period(period)
{
    char* out;
    std::size_t out_len;
    int res = oath_base32_decode(m_secret_base32.data(),
                                 m_secret_base32.size(),
                                 &out,
                                 &out_len);
    check_oath_result(res);

    m_secret.resize(out_len);
    std::memcpy(m_secret.data(), out, out_len);
    std::free(out);
}

std::string TOTP::generate() const
{
    std::string out;
    out.resize(m_digits);
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    int res = oath_totp_generate2(reinterpret_cast<const char*>(m_secret.data()),
                                  m_secret.size(),
                                  now,
                                  m_period,
                                  OATH_TOTP_DEFAULT_START_TIME,
                                  m_digits,
                                  0,
                                  out.data());
    check_oath_result(res);

    return out;
}

TOTP::~TOTP()
{
    zero_data(m_secret);
    zero_data(m_secret_base32);
    zero_data(m_issuer);
    zero_data(m_label);
    m_period = 0;
    m_digits = 0;
    m_algorithm = Algorithm::SHA1;
    visible = false;
}

}  // namespace otpd
