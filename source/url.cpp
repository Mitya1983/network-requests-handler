#include "include/url.hpp"
#include "include/network_utility.hpp"
#include "include/network_schemes.hpp"
#include "include/network_error.hpp"

#if defined(__linux__)
  #include <netdb.h>
#endif

#include "include/udp_socket.hpp"
#include "include/request/impl/dns_request.hpp"
#include "include/response/impl/dns_response.hpp"

#include <unordered_map>
#include <regex>
#include <cstring>

#include <iostream>

namespace {

    inline const std::unordered_map< char, std::string > g_percentage_encoding = {
        {' ',  "%20"},
        {'!',  "%21"},
        {'@',  "%40"},
        {'#',  "%23"},
        {'$',  "%24"},
        {'%',  "%25"},
        {'&',  "%26"},
        {'*',  "%2A"},
        {'(',  "%28"},
        {')',  "%29"},
        {'+',  "%2B"},
        {'=',  "%3D"},
        {'[',  "%5B"},
        {']',  "%5D"},
        {':',  "%3A"},
        {';',  "%3B"},
        {'\'', "%27"},
        {',',  "%2C"},
        {'/',  "%2F"},
        {'?',  "%3F"},
    };

}  //End of unnamed namespace

mt::network::Url::Url(const std::string& p_url) {
    bool scheme_parsed{false};
    if (p_url.find("://") == std::string::npos) {
        scheme_parsed = true;
    }
    bool user_name_parsed{true};
    bool user_password_parsed{true};
    if (p_url.find('@') != std::string::npos) {
        user_name_parsed = false;
    }
    bool host_parsed{false};
    bool port_parsed{true};
    bool path_parsed{false};
    bool query_parsed{false};
    for (int64_t index = 0, length = std::ssize(p_url); index < length; ++index) {
        if (not scheme_parsed) {
            if (p_url[index] == ':') {
                scheme_parsed = true;
                ++ ++index;
                continue;
            }
            m_scheme += p_url[index];
            continue;
        }
        if (not user_name_parsed) {
            if (p_url[index] == ':') {
                user_name_parsed = true;
                user_password_parsed = false;
                continue;
            }
            if (p_url[index] == '@') {
                user_name_parsed = true;
                continue;
            }
            m_user_name += p_url[index];
        }
        if (not user_password_parsed) {
            if (p_url[index] == '@') {
                user_password_parsed = true;
                continue;
            }
            m_user_password += p_url[index];
        }
        if (not host_parsed) {
            if (p_url[index] == ':') {
                port_parsed = false;
                host_parsed = true;
                continue;
            }
            if (p_url[index] == '/') {
                host_parsed = true;
                continue;
            }
            m_host += p_url[index];
            continue;
        }
        if (not port_parsed) {
            if (p_url[index] == '/') {
                port_parsed = true;
                continue;
            }
            m_port += p_url[index];
            continue;
        }
        if (not path_parsed) {
            if (p_url[index] == '?') {
                path_parsed = true;
                continue;
            }
            m_path += p_url[index];
            continue;
        }
        if (not query_parsed) {
            if (p_url[index] == '#') {
                query_parsed = true;
                continue;
            }
            m_query += p_url[index];
            continue;
        }
        //If we reach this point only fragment left
        m_fragment += p_url[index];
    }
    if (m_path.empty()) {
        m_path = "/";
    }
    if (const auto port = schemes::getNetworkSchemeDefaultPort(m_scheme); port != 0) {
        setPort(port, std::endian::native);
    }
    m_valid = true;
}

void mt::network::Url::setScheme(std::string p_scheme) {
    if (p_scheme.at(0) < 'A' || p_scheme.at(0) > 'z' || (p_scheme.at(0) > 'Z' && p_scheme.at(0) < 'a')) {
        throw NetworkException(UrlErrors::Unsupported_scheme);
    }
    if (const auto not_allowed_char = p_scheme.find_first_not_of("+-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"); not_allowed_char != std::string::npos) {
        throw NetworkException(UrlErrors::Unsupported_scheme);
    }
    m_scheme = std::move(p_scheme);
    if (const auto port = schemes::getNetworkSchemeDefaultPort(m_scheme); port != 0) {
        setPort(port);
    }
}

void mt::network::Url::setAuthority(std::string p_host, std::string p_user_name, std::string p_user_password) {
    if (p_host.size() > 253) {
        throw NetworkException(UrlErrors::Bad_host_size);
    }
    if (const auto not_allowed_char = p_host.find_first_not_of("-.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"); not_allowed_char != std::string::npos) {
        throw NetworkException(UrlErrors::Bad_host_format);
    }
    if (p_host.front() == '-' || p_host.back() == '-') {
        throw NetworkException(UrlErrors::Bad_host_format);
    }
    m_host = std::move(p_host);
    m_user_name = std::move(p_user_name);
    size_t char_to_encode = 0;
    while (true) {
        char_to_encode = m_user_name.find_first_of(" !@#$%&*()+=[];:\',/?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        m_user_name.replace(char_to_encode, 1, g_percentage_encoding.at(m_user_name.at(char_to_encode)));
        ++char_to_encode;
    }
    m_user_password = std::move(p_user_password);
    char_to_encode = 0;
    while (true) {
        char_to_encode = m_user_password.find_first_of(" !@#$%&*()+=[];:\',/?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        m_user_password.replace(char_to_encode, 1, g_percentage_encoding.at(m_user_name.at(char_to_encode)));
        ++char_to_encode;
    }
}

void mt::network::Url::addHostIP(std::string p_ip) { m_host_ip.emplace_back(std::move(p_ip)); }

void mt::network::Url::setPort(uint16_t p_port, const std::endian p_endian) {
    if (p_endian == std::endian::big) {
        m_port = p_port;
        return;
    }
    auto ptr = reinterpret_cast< std::byte* >(&p_port);
    std::swap(ptr[0], ptr[1]);
    m_port = *reinterpret_cast< uint16_t* >(ptr);
}

void mt::network::Url::setPort(const std::string& p_port) { m_port = std::stoi(p_port); }

void mt::network::Url::setPath(std::string p_path) {
    m_path = std::move(p_path);
    size_t char_to_encode = 0;
    while (true) {
        char_to_encode = m_path.find_first_of(" !@#$%&*()+=[];:\',?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        m_path.replace(char_to_encode, 1, g_percentage_encoding.at(m_path.at(char_to_encode)));
        ++char_to_encode;
    }
}

void mt::network::Url::setQuery(std::string p_query) {
    m_query = std::move(p_query);
    size_t char_to_encode = 0;
    while (true) {
        char_to_encode = m_query.find_first_of(" !@#$%*()+[]:\',/?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        m_query.replace(char_to_encode, 1, g_percentage_encoding.at(m_query.at(char_to_encode)));
        ++char_to_encode;
    }
}

void mt::network::Url::setFragment(std::string p_fragment) {
    m_fragment = std::move(p_fragment);
    size_t char_to_encode = 0;
    while (true) {
        char_to_encode = m_fragment.find_first_of(" !@#$%&*()+=[];:\',/?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        m_fragment.replace(char_to_encode, 1, g_percentage_encoding.at(m_fragment.at(char_to_encode)));
        ++char_to_encode;
    }
}

auto mt::network::Url::scheme() const noexcept -> const std::string& { return m_scheme; }

auto mt::network::Url::userName() const noexcept -> const std::string& { return m_user_name; }

auto mt::network::Url::userPassword() const noexcept -> const std::string& { return m_user_password; }

auto mt::network::Url::host() const noexcept -> const std::string& { return m_host; }

auto mt::network::Url::hostIP() const noexcept -> Ipv4 { return m_host_ip.empty() ? Ipv4{} : m_host_ip[0]; }

auto mt::network::Url::hostIPList() const noexcept -> const std::vector< Ipv4 >& { return m_host_ip; }

auto mt::network::Url::port() const noexcept -> std::string {
    if (m_port == 0) {
        return {};
    }
    return std::to_string(m_port);
}

auto mt::network::Url::port_local_byte_order() const noexcept -> uint16_t {
    if constexpr (std::endian::native == std::endian::big) {
        return m_port;
    }
    auto port = m_port;
    const auto ptr = reinterpret_cast< std::byte* >(&port);
    std::swap(ptr[0], ptr[1]);
    return *reinterpret_cast< uint16_t* >(ptr);
}

auto mt::network::Url::port_network_byte_order() const noexcept -> uint16_t { return m_port; }

auto mt::network::Url::path() const noexcept -> const std::string& { return m_path; }

auto mt::network::Url::query() const noexcept -> const std::string& { return m_query; }

auto mt::network::Url::fragment() const noexcept -> const std::string& { return m_fragment; }

auto mt::network::Url::composeUrl() const -> std::string {
    std::string uri;
    if (not m_scheme.empty()) {
        uri += m_scheme;
        uri += ':';
    }
    if (not m_host.empty() || not m_host_ip.empty()) {
        uri += "//";
        if (not m_user_name.empty()) {
            uri += m_user_name;
            if (not m_user_password.empty()) {
                uri += ':';
                uri += m_user_password;
            }
            uri += '@';
        }
        if (not m_host.empty()) {
            uri += m_host;
        } else {
            uri += std::string{m_host_ip.at(0)};
        }
        if (m_port != 0) {
            uri += ':';
            uri += std::to_string(m_port);
        }
    }
    uri += m_path;
    if (not m_query.empty()) {
        uri += '?';
        uri += m_query;
    }
    if (not m_fragment.empty()) {
        uri += '#';
        uri += m_fragment;
    }
    return uri;
}

auto mt::network::Url::valid() const noexcept -> bool { return m_valid; }

auto mt::network::Url::resolved() const noexcept -> bool { return m_resolved; }

auto mt::network::Url::error() const noexcept -> std::error_code {
    return m_error;
}

void mt::network::Url::resolve() {
    DnsRequest request{m_host};
    request.processRequest();
    const auto response = request.response();
    if (const auto error = response->error(); error){
        m_error = error;
        return;
    }
    m_host_ip = std::move(response->resolved_ips());
    m_resolved = true;
}
