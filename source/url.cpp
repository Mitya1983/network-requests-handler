#include "include/url.hpp"
#include "include/network_utility.hpp"
#include "include/network_schemes.hpp"
#include "include/network_error.hpp"

#if defined(__linux__)
  #include <netdb.h>
#endif

#include <unordered_map>
#include <regex>
#include <cstring>

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

void mt::network::Url::resolve() {
    auto host = m_host;
    if (host.find("www.") == 0) {
        host.erase(0, 4);
    }
    const auto resolver_results = gethostbyname(host.c_str());
    if (resolver_results == nullptr) {
        switch (h_errno) {
            case HOST_NOT_FOUND: {
                throw NetworkException{UrlErrors::Not_found_error};
            }
            case TRY_AGAIN: {
                throw NetworkException{UrlErrors::Try_again_error};
            }
            case NO_RECOVERY: {
                throw NetworkException{UrlErrors::No_recovery_error};
            }
            case NO_DATA: {
                throw NetworkException{UrlErrors::No_data_error};
            }
            default: {
                throw NetworkException{UrlErrors::Unknown_error};
            }
        }
    }
    uint8_t index = 0;
    while (true) {
        const auto address = resolver_results->h_addr_list[index];
        if (address == nullptr) {
            break;
        }
        uint32_t ip;
        std::memmove(&ip, address, 4);
        m_host_ip.emplace_back(ip);
        ++index;
    }

    // #include <iostream>
    // #include <cstring>
    // #include <sys/socket.h>
    // #include <arpa/inet.h>
    // #include <unistd.h>
    //
    // #define DNS_PORT 53
    // #define DNS_SERVER "8.8.8.8" // Google's public DNS server
    //
    // #pragma pack(push, 1)
    // struct DNSHeader {
    //     uint16_t id; // Identification
    //     uint16_t flags; // Flags
    //     uint16_t qdcount; // Number of questions
    //     uint16_t ancount; // Number of answers
    //     uint16_t nscount; // Number of authority records
    //     uint16_t arcount; // Number of additional records
    // };
    //
    // struct Question {
    //     uint16_t qtype;
    //     uint16_t qclass;
    // };
    // #pragma pack(pop)
    //
    // void build_dns_query(const std::string& hostname, uint8_t* buffer, size_t& query_size) {
    //     DNSHeader* dns_header = reinterpret_cast<DNSHeader*>(buffer);
    //     dns_header->id = htons(0x1234); // Random ID
    //     dns_header->flags = htons(0x0100); // Standard query
    //     dns_header->qdcount = htons(1); // One question
    //     dns_header->ancount = 0;
    //     dns_header->nscount = 0;
    //     dns_header->arcount = 0;
    //
    //     uint8_t* qname = buffer + sizeof(DNSHeader);
    //     const char* hostname_cstr = hostname.c_str();
    //     while (*hostname_cstr) {
    //         const char* dot = strchr(hostname_cstr, '.');
    //         if (!dot) dot = hostname_cstr + strlen(hostname_cstr);
    //         *qname++ = dot - hostname_cstr;
    //         memcpy(qname, hostname_cstr, dot - hostname_cstr);
    //         qname += dot - hostname_cstr;
    //         hostname_cstr = (*dot) ? dot + 1 : dot;
    //     }
    //     *qname++ = 0; // End of hostname
    //
    //     Question* question = reinterpret_cast<Question*>(qname);
    //     question->qtype = htons(1); // Type A
    //     question->qclass = htons(1); // Class IN
    //
    //     query_size = qname + sizeof(Question) - buffer;
    // }
    //
    // int main() {
    //     int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    //     if (sock < 0) {
    //         perror("socket");
    //         return 1;
    //     }
    //
    //     struct sockaddr_in dest;
    //     dest.sin_family = AF_INET;
    //     dest.sin_port = htons(DNS_PORT);
    //     inet_pton(AF_INET, DNS_SERVER, &dest.sin_addr);
    //
    //     uint8_t buffer[512];
    //     size_t query_size;
    //     build_dns_query("example.com", buffer, query_size);
    //
    //     if (sendto(sock, buffer, query_size, 0, (struct sockaddr*)&dest, sizeof(dest)) < 0) {
    //         perror("sendto");
    //         close(sock);
    //         return 1;
    //     }
    //
    //     socklen_t len = sizeof(dest);
    //     ssize_t response_size = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&dest, &len);
    //     if (response_size < 0) {
    //         perror("recvfrom");
    //         close(sock);
    //         return 1;
    //     }
    //
    //     std::cout << "Received DNS response of size " << response_size << " bytes" << std::endl;
    //
    //     close(sock);
    //     return 0;
    // }

    // https://cabulous.medium.com/dns-message-how-to-read-query-and-response-message-cfebcb4fe817

    // https://linux.die.net/man/3/res_query
}
