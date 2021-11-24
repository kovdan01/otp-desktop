#ifndef OTP_DESKTOP_CRYPTO_HPP_
#define OTP_DESKTOP_CRYPTO_HPP_

#include <utils.hpp>

#include <span>
#include <string_view>
#include <vector>

namespace otpd
{

std::vector<byte_t> encrypt_data(std::span<const byte_t> plaintext, std::string_view password);
std::vector<byte_t> decrypt_data(std::span<const byte_t> data, std::string_view password);

}  // namespace otpd

#endif  // OTP_DESKTOP_CRYPTO_HPP_
