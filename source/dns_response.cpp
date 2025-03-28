// Created by Mitia Tristan on 02.02.25.

#include "include/response/impl/dns_response.hpp"

#include "include/network_error.hpp"
#include "include/network_utility.hpp"
#include "utility/include/utility.hpp"

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
            while (*iter != uint8_t{0}) {
                const auto size = static_cast< uint8_t >(*iter);
                ++iter;
                name += mt::utility::string(iter, iter + size);
                iter += size;
                if (*iter != uint8_t{0}) {
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

    std::string nameFromPointer(std::vector< std::byte >::iterator iter, std::vector< std::byte >& data);
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
    if (const auto error = static_cast< std::byte >(dns_header.return_code & uint16_t{0x000F}); error != uint8_t{0}) {
        if (error < uint8_t{9}) {
            m_error = makeError(static_cast< DnsErrors >(error));
        } else {
            m_error = makeError(DnsErrors::Unknown_error);
        }
        return;
    }
    Question question{iter};
    while (iter != m_raw_data.end()) {
        if (*iter == uint8_t{192}) {
            ++iter;
            if (auto name = nameFromPointer(m_raw_data.begin() + static_cast< int8_t >(*iter), m_raw_data); m_aliases.empty() or name != m_aliases.back()) {
                m_aliases.push_back(std::move(name));
            }
            ++iter;
        }
        const auto response_type = utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
        iter += 2;
        [[maybe_unused]] const auto response_class = utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
        iter += 2;
        [[maybe_unused]] const auto valid_till = utility::toHostByteOrder(*reinterpret_cast< uint32_t* >(&*iter));
        iter += 4;
        const auto data_length = utility::toHostByteOrder(*reinterpret_cast< uint16_t* >(&*iter));
        iter += 2;
        switch (response_type) {
            case 1: {
                m_ipv4s.emplace_back(*reinterpret_cast< uint32_t* >(&*iter));
                iter += sizeof(int32_t);
                break;
            }
            case 2: {
                break;
            }
            case 5: {
                auto end = iter + data_length;
                std::string alias;
                while (iter != end) {
                    if (*iter == uint8_t{192}) {
                        ++iter;
                        alias += nameFromPointer(m_raw_data.begin() + static_cast< int8_t >(*iter), m_raw_data);
                        ++iter;
                        continue;
                    }
                    const auto length = static_cast< uint8_t >(*iter);
                    ++iter;
                    alias += mt::utility::string(iter, iter + length);
                    iter += length;
                    if (*iter != uint8_t{0}) {
                        alias += '.';
                    } else {
                        ++iter;
                        break;
                    }
                }
                if (m_aliases.back() != alias) {
                    m_aliases.push_back(std::move(alias));
                }
                break;
            }
            case 6:
            case 12:
            case 15:
            case 16:
            case 28:
            case 33:
            case 35:
            case 257:
            default: {
                break;
            }
        }
    }
}

auto mt::network::DnsResponse::error() const -> const std::error_code& { return m_error; }

auto mt::network::DnsResponse::resolved_ips() -> std::vector< Ipv4 >& { return m_ipv4s; }

namespace {
    std::string nameFromPointer(std::vector< std::byte >::iterator iter, std::vector< std::byte >& data) {  //NOLINT
        std::string result;
        while (*iter != uint8_t{0}) {
            if (*iter == uint8_t{192}) {
                ++iter;
                result += nameFromPointer(data.begin() + static_cast< int8_t >(*iter), data);
                // ++iter;
                break;
            }
            const auto length = static_cast< uint8_t >(*iter);
            ++iter;
            result += mt::utility::string(iter, iter + length);
            iter += length;
            if (*iter != uint8_t{0}) {
                result += '.';
            }
        }
        return result;
    }
}  // namespace