// Created by Mitia Tristan on 20.12.24.
#include "include/ipv4.hpp"
#include "include/network_error.hpp"
#include <regex>

namespace {
    constexpr auto g_ip4_regex_pattern = "^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$";
}

mt::network::Ipv4::Ipv4(const std::string& p_ip) {
    const std::regex regex(g_ip4_regex_pattern);
    std::smatch ip_check_result;
    std::regex_match(p_ip, ip_check_result, regex);
    if (ip_check_result.empty()) {
        throw NetworkException(UrlErrors::Bad_ip_format);
    }
    std::string part;
    int32_t partIndex = 0;
    for (int64_t index = 0, length = std::ssize(p_ip); index < length; ++index) {
        if (p_ip[index] == '.') {
            auto value = std::stoi(part);
            m_ip[partIndex] = *reinterpret_cast<std::byte*>(&value);
            part.clear();
            ++partIndex;
            continue;
        }
        part += p_ip[index];
    }
    auto value = std::stoi(part);
    m_ip[partIndex] = *reinterpret_cast<std::byte*>(&value);
}

mt::network::Ipv4::Ipv4(std::string&& p_ip) : Ipv4(p_ip) { }

mt::network::Ipv4::Ipv4(const uint32_t p_ip, const std::endian p_endian) {
    const auto ptr = reinterpret_cast<const std::byte*>(&p_ip);
    if (p_endian == std::endian::little) {
        m_ip[0] = ptr[3];
        m_ip[1] = ptr[2];
        m_ip[2] = ptr[1];
        m_ip[3] = ptr[0];
        return;
    }
    m_ip[0] = ptr[0];
    m_ip[1] = ptr[1];
    m_ip[2] = ptr[2];
    m_ip[3] = ptr[3];
}

mt::network::Ipv4::operator uint32_t() const {
    return *reinterpret_cast<const uint32_t*>(m_ip.data());
}

mt::network::Ipv4::operator std::string() const {
    std::string ip;
    ip += std::to_string(std::to_integer< uint32_t >(m_ip[0]));
    ip += '.';
    ip += std::to_string(std::to_integer< uint32_t >(m_ip[1]));
    ip += '.';
    ip += std::to_string(std::to_integer< uint32_t >(m_ip[2]));
    ip += '.';
    ip += std::to_string(std::to_integer< uint32_t >(m_ip[3]));
    return ip;
}