// Created by Mitia Tristan on 02.02.25.

#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_RESPONSE_IMPL_DNS_REAPONSE_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_RESPONSE_IMPL_DNS_REAPONSE_HPP

#include "include/ipv4.hpp"

#include <vector>
#include <system_error>

namespace mt::network {
    class DnsResponse {
    public:
        explicit DnsResponse(std::vector< std::byte > data, uint16_t p_id);
        DnsResponse(const DnsResponse& p_other) = delete;
        DnsResponse(DnsResponse&& p_other) noexcept = delete;
        DnsResponse& operator=(const DnsResponse& p_other) = delete;
        DnsResponse& operator=(DnsResponse&& p_other) noexcept = delete;
        ~DnsResponse() = default;

        [[nodiscard]] auto error() const -> const std::error_code&;
        [[nodiscard]] auto resolved_ips() -> std::vector< Ipv4 >&;

    private:
        std::vector< std::byte > m_raw_data;  // Consider not to store this
        std::vector< std::string > m_aliases;
        std::vector< Ipv4 > m_ipv4s;
        std::error_code m_error;

        uint16_t m_id;
    };
}  // namespace mt::network

#endif  //NETWORK_REQUEST_HANDLER_INCLUDE_RESPONSE_IMPL_DNS_REAPONSE_HPP
