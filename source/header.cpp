#include "include/http/header.hpp"

#include "include/network_error.hpp"
#include "include/network_utility.hpp"

#include <algorithm>

mt::network::http::Header::Header(std::string_view::const_iterator& p_begin, const std::string_view::const_iterator p_end) {
    bool name_parsed{false};
    bool l_value{false};
    bool carriage_return{false};
    while (p_begin != p_end) {
        if (*p_begin == ':') {
            if (not name_parsed) {
                name_parsed = true;
                ++p_begin;
                continue;
            }
        }
        if (*p_begin == '\r') {
            carriage_return = true;
            ++p_begin;
            continue;
        }
        if (*p_begin == ' ' and name_parsed and not l_value) {
            ++p_begin;
            continue;
        }
        if (*p_begin == '\n') {
            if (carriage_return) {
                ++p_begin;
                return;
            }
            if (not l_value) {
                throw NetworkException(HttpErrors::Bad_http_header_format);
            }
        }
        if (name_parsed and not l_value) {
            l_value = true;
        }
        if (l_value) {
            value += *p_begin;
        } else {
            name += *p_begin;
        }
        ++p_begin;
    }
}

mt::network::http::HttpHeaders::HttpHeaders(std::string_view p_data) {
    auto iter = p_data.begin();
    while (iter != p_data.end()) {
        m_headers.emplace_back(iter, p_data.end());
    }
}

void mt::network::http::HttpHeaders::addHeader(Header p_header) { m_headers.emplace_back(std::move(p_header)); }

auto mt::network::http::HttpHeaders::headerValue(const std::string& p_header_name) const -> std::optional< std::string > {
    const auto found = std::ranges::find_if(m_headers, [p_header_name](const Header& header) -> bool {
        return header.name == p_header_name || header.name == mt::network::utility::capitalizeHttpHeader(p_header_name);
    });
    if (found == m_headers.cend()) {
        return std::nullopt;
    }
    return found->value;
}

auto mt::network::http::HttpHeaders::empty() const -> bool { return m_headers.empty(); }

auto mt::network::http::HttpHeaders::begin() noexcept -> std::vector< Header >::iterator { return m_headers.begin(); }

auto mt::network::http::HttpHeaders::begin() const noexcept -> std::vector< Header >::const_iterator { return m_headers.begin(); }

auto mt::network::http::HttpHeaders::cbegin() const noexcept -> std::vector< Header>::const_iterator {
    return m_headers.cbegin();
}

auto mt::network::http::HttpHeaders::end() noexcept -> std::vector< Header >::iterator { return m_headers.end(); }

auto mt::network::http::HttpHeaders::end() const noexcept -> std::vector< Header >::const_iterator { return m_headers.end(); }

auto mt::network::http::HttpHeaders::cend() const noexcept -> std::vector< Header>::const_iterator {
    return m_headers.cend();
}
