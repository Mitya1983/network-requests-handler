// Created by Mitia Tristan on 02.02.25.

#include "include/request/impl/dns_request.hpp"
#include "include/ipv4.hpp"
#include "include/network_utility.hpp"
#include "include/response/impl/dns_response.hpp"

#include "sockets/include/udp_socket.hpp"
#include "utility/include/utility.hpp"

namespace {
    struct DNSHeader {
        uint16_t id{0};
        uint16_t flags{0};
        uint16_t questions_count{0};
        uint16_t answers_count{0};
        uint16_t authority_records_count{0};
        uint16_t additional_records_count{0};
    };

    struct Question {
        uint16_t question_type{0};
        uint16_t question_class{0};
    };
}  // namespace

mt::network::DnsRequest::DnsRequest(const std::string& p_host_name) {

    m_dns_servers = {"",
                     "8.8.8.8",
                     "8.8.4.4",
                     "1.1.1.1",
                     "1.0.0.1",
                     "208.67.222.222",
                     "208.67.220.220",
                     "9.9.9.9",
                     "149.112.112.112",
                     "8.26.56.26",
                     "8.20.247.20",
                     "185.228.168.9",
                     "185.228.169.9"};

    if (auto&& default_dns_ip = utility::getLocalDnsIp(); not default_dns_ip.empty()) {
        m_dns_servers[0] = std::move(default_dns_ip);
    }
    DNSHeader dns_header{};
    dns_header.id = m_id = mt::utility::generateRandomInteger(1, std::numeric_limits< uint16_t >::max());
    dns_header.flags = mt::network::utility::toNetworkByteOrder(uint16_t{0x0100});;
    dns_header.questions_count = mt::network::utility::toNetworkByteOrder(uint16_t{0x0001});
    dns_header.answers_count = 0;
    dns_header.authority_records_count = 0;
    dns_header.additional_records_count = 0;
    std::copy_n(reinterpret_cast< std::byte* >(&dns_header), sizeof(DNSHeader), std::back_inserter(m_request_data));
    m_request_data.push_back(std::byte{0});
    uint8_t label_size{0};
    for (auto ch: p_host_name) {
        if (ch == '.') {
            m_request_data[std::ssize(m_request_data) - label_size - 1] = static_cast< std::byte >(label_size);
            label_size = 0;
            m_request_data.push_back(std::byte{0});
            continue;
        }
        m_request_data.push_back(static_cast< std::byte >(ch));
        ++label_size;
    }
    m_request_data[std::ssize(m_request_data) - label_size - 1] = static_cast< std::byte >(label_size);
    m_request_data.push_back(std::byte{0});
    Question question{};
    question.question_type = mt::network::utility::toNetworkByteOrder(uint16_t{1});
    question.question_class = mt::network::utility::toNetworkByteOrder(uint16_t{1});
    std::copy_n(reinterpret_cast< std::byte* >(&question), sizeof(Question), std::back_inserter(m_request_data));
}

void mt::network::DnsRequest::processRequest() {

    for (const auto& dns_server: m_dns_servers) {
        if (dns_server.empty()) {
            continue;
        }
        mt::sockets::UdpSocket socket;
        if (const auto error = socket.error(); error) {
            m_error = error;
            return;
        }
        socket.setDestinationHost(uint32_t{Ipv4{dns_server}});
        socket.setDestinationPort(utility::toNetworkByteOrder(uint16_t{53}));
        socket.setLocalHost(uint32_t{Ipv4{"0.0.0.0"}});
        socket.setLocalPort(utility::toNetworkByteOrder(static_cast<uint16_t>(mt::utility::generateRandomInteger(49152, std::numeric_limits<uint16_t>::max()))));
        socket.bind();
        socket.write(m_request_data);
        if (const auto error = socket.error(); error) {
            m_error = error;
            continue;
        }
        auto&& response = socket.read();
        if (const auto error = socket.error(); error) {
            m_error = error;
            continue;
        }
        m_response = std::make_shared< DnsResponse >(std::move(response), m_id);
        if (not m_response->error()) {
            return;
        }
    }
    throw std::runtime_error("Host not found");
}

auto mt::network::DnsRequest::error() const -> const std::error_code& { return m_error; }

auto mt::network::DnsRequest::response() const -> std::shared_ptr< DnsResponse > { return m_response; }
