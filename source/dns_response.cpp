// Created by Mitia Tristan on 02.02.25.

#include "include/response/impl/dns_response.hpp"

#include "include/network_error.hpp"
#include "include/network_utility.hpp"

namespace {
    struct DNSHeader {
        explicit DNSHeader(std::vector< std::byte >::iterator& iter) {
            id = *reinterpret_cast< uint16_t* >(&*iter);
            ++ ++iter;
            return_code = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
            questions_count = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
            answers_count = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
            authority_records_count = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
            additional_records_count = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
        }

        uint16_t id{0};
        uint16_t return_code{0};
        uint16_t questions_count{0};
        uint16_t answers_count{0};
        uint16_t authority_records_count{0};
        uint16_t additional_records_count{0};
    };

    struct Question {
        explicit Question(std::vector< std::byte >::iterator& iter) {
            while (not mt::network::utility::equal(*iter, 0)) {
                const auto size = static_cast< uint8_t >(*iter);
                ++iter;
                name += mt::network::utility::string(iter, iter + size);
                iter += size;
                if (not mt::network::utility::equal(*iter, 0)) {
                    name += '.';
                }
            }
            ++iter;
            type = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
            class_ = mt::network::utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
            ++ ++iter;
        }

        std::string name;
        uint16_t type{0};
        uint16_t class_{0};
    };
}  // namespace

mt::network::DnsResponse::DnsResponse(std::vector< std::byte > data, const uint16_t p_id) :
    m_raw_data(std::move(data)),
    m_id(p_id) {
    if (std::ssize(m_raw_data) < static_cast< int64_t >(sizeof(DNSHeader))) {
        m_error = makeError(DnsErrors::Incorrect_response_size);
        return;
    }
    auto iter = m_raw_data.begin();
    const DNSHeader dns_header{iter};
    if (dns_header.id != m_id) {
        m_error = makeError(DnsErrors::Response_id_missmatch);
        return;
    }
    if (const auto error = static_cast<std::byte>(dns_header.return_code & uint16_t{0x000F}); not utility::equal(error, 0)) {
        if (utility::less(error, 9)) {
            m_error = makeError(static_cast<DnsErrors>(error));
        } else {
            m_error = makeError(DnsErrors::Unknown_error);
        }
        return;
    }
    Question question{iter};

    for (uint16_t index = 0; index < dns_header.answers_count; ++index) {
        //NOTE: For now only ip are substantial
        iter+= 11;
        if (utility::equal(*iter, sizeof(uint32_t))) {
            ++iter;
            m_ipv4s.emplace_back(*reinterpret_cast< uint32_t* >(&*iter));
            iter += sizeof(uint32_t);
        }
    }
}

auto mt::network::DnsResponse::error() const -> const std::error_code& { return m_error; }

auto mt::network::DnsResponse::resolved_ips() -> std::vector< Ipv4 >& {
    return m_ipv4s;
}