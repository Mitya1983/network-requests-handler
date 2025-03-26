// Created by Mitia Tristan on 20.12.24.

#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_IPV4_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_IPV4_HPP

#include <cstdint>
#include <string>
#include <array>

namespace mt::network {
    class Ipv4 {
    public:
        Ipv4() = default;
        explicit Ipv4(const std::string& p_ip);
        explicit Ipv4(std::string&& p_ip);
        explicit Ipv4(uint32_t p_ip, std::endian p_endian = std::endian::big);
        Ipv4(const Ipv4& p_other) = default;
        Ipv4(Ipv4&& p_other) noexcept = default;
        Ipv4& operator=(const Ipv4& p_other) = default;
        Ipv4& operator=(Ipv4&& p_other) noexcept = default;
        explicit operator uint32_t() const;
        explicit operator std::string() const;
        ~Ipv4() = default;
    private:
        std::array<std::byte, 4> m_ip{};
    };
}
#endif //NETWORK_REQUEST_HANDLER_INCLUDE_IPV4_HPP
