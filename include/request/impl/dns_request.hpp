// Created by Mitia Tristan on 02.02.25.

#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_REQUEST_IMPL_DNS_REQUEST_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_REQUEST_IMPL_DNS_REQUEST_HPP

#include <string>
#include <vector>
#include <memory>
#include <array>

namespace mt::network {
    class DnsResponse;

    class DnsRequest {
    public:
        explicit DnsRequest(const std::string& p_host_name);
        DnsRequest(const DnsRequest& p_other) = delete;
        DnsRequest(DnsRequest&& p_other) noexcept = delete;
        DnsRequest& operator=(const DnsRequest& p_other) = delete;
        DnsRequest& operator=(DnsRequest&& p_other) noexcept = delete;
        ~DnsRequest() = default;

        void processRequest();

        [[nodiscard]] auto error() const -> const std::error_code&;
        [[nodiscard]] auto response() const -> std::shared_ptr< DnsResponse >;

    private:
        std::array< std::string, 13 > m_dns_servers;
        std::vector< std::byte > m_request_data;
        std::error_code m_error;
        std::shared_ptr< DnsResponse > m_response;
        uint16_t m_id{0};
    };
}  // namespace mt::network

#endif  //NETWORK_REQUEST_HANDLER_INCLUDE_REQUEST_IMPL_DNS_REQUEST_HPP
