#include "include/http/http_header.hpp"

#include "include/network_utility.hpp"

#include <algorithm>

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

auto mt::network::http::HttpHeaders::begin() noexcept -> std::vector< Header>::iterator {
    return m_headers.begin();
}

auto mt::network::http::HttpHeaders::cbegin() const noexcept -> std::vector< Header>::const_iterator {
    return m_headers.cbegin();
}

auto mt::network::http::HttpHeaders::end() noexcept -> std::vector< Header>::iterator {
    return m_headers.end();
}

auto mt::network::http::HttpHeaders::cend() const noexcept -> std::vector< Header>::const_iterator {
    return m_headers.cend();
}
