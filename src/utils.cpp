#include <utils.hpp>

namespace otpd
{

Exception::~Exception() = default;

CryptoException::~CryptoException() = default;

TOTPException::~TOTPException() = default;

ParserException::~ParserException() = default;

void zero_data(std::span<byte_t> data) noexcept
{
    // memset_s is available in C but not in C++
    // use std::fill with volatile pointers as a workaround
    volatile byte_t* from = data.data();
    volatile byte_t* to = data.data() + data.size();
    std::fill(from, to, 0);
}

void zero_data(std::string& data) noexcept
{
    zero_data({reinterpret_cast<byte_t*>(data.data()), data.size()});
}

}  // namespace otpd
