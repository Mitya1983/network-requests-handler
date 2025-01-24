#ifndef HTTP_HEADER_HPP
#define HTTP_HEADER_HPP

#include "network_error.hpp"

#include "http/concepts.hpp"

#include <string>
#include <vector>
#include <optional>
#include <iterator>
#include <ranges>

namespace mt::network::http {

    struct Header {
        std::string name;
        std::string value;

        explicit Header(std::string name_, std::string value_) :
            name(std::move(name_)),
            value(std::move(value_)) { }

        explicit Header(std::indirectly_readable auto& begin, std::indirectly_readable auto end)
            requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*end) > > or concepts::parse_compatible< std::decay_t< decltype(*begin) > >
        {
            bool name_parsed{false};
            bool l_value{false};
            bool carriage_return{false};
            while (begin != end) {
                char ch;
                if constexpr (concepts::convertible_to_char< std::decay_t< decltype(*begin) > >) {
                    ch = static_cast< char >(*begin);
                } else {
                    ch = char{*begin};
                }
                if (ch == ':') {
                    name_parsed = true;
                    ++begin;
                    continue;
                }
                if (ch == '\r') {
                    carriage_return = true;
                    ++begin;
                    continue;
                }
                if (ch == ' ' and name_parsed and not l_value) {
                    ++begin;
                    continue;
                }
                if (ch == '\n') {
                    if (carriage_return) {
                        ++begin;
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
                    value += ch;
                } else {
                    name += ch;
                }
                ++begin;
            }
        }
    };

    class HttpHeaders {
    public:
        HttpHeaders() = default;
        explicit HttpHeaders(std::indirectly_readable auto begin, std::indirectly_readable auto end)
            requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*end) > > or concepts::parse_compatible< std::decay_t< decltype(*begin) > >;
        explicit HttpHeaders(std::ranges::input_range auto&& range)
            requires std::is_same_v< std::decay_t< decltype(*range.begin()) >, std::decay_t< decltype(*range.end()) > >
                     or concepts::parse_compatible< std::decay_t< decltype(*range.begin()) > >;
        HttpHeaders(const HttpHeaders& p_other) = default;
        HttpHeaders(HttpHeaders&& p_other) noexcept = default;
        HttpHeaders& operator=(const HttpHeaders& p_other) = default;
        HttpHeaders& operator=(HttpHeaders&& p_other) noexcept = default;
        ~HttpHeaders() = default;

        void addHeader(Header p_header);

        [[nodiscard]] auto headerValue(const std::string& p_header_name) const -> std::optional< std::string >;
        [[nodiscard]] auto empty() const -> bool;
        [[nodiscard]] auto begin() noexcept -> std::vector< Header >::iterator;
        [[nodiscard]] auto cbegin() const noexcept -> std::vector< Header >::const_iterator;
        [[nodiscard]] auto end() noexcept -> std::vector< Header >::iterator;
        [[nodiscard]] auto cend() const noexcept -> std::vector< Header >::const_iterator;

    private:
        std::vector< Header > m_headers;
    };

    HttpHeaders::HttpHeaders(std::indirectly_readable auto begin, std::indirectly_readable auto end)
        requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*end) > > or concepts::parse_compatible< std::decay_t< decltype(*begin) > >
    {
        while (begin != end) {
            m_headers.emplace_back(begin, end);
        }
    }

    HttpHeaders::HttpHeaders(std::ranges::input_range auto&& range)
        requires std::is_same_v< std::decay_t< decltype(*range.begin()) >, std::decay_t< decltype(*range.end()) > >
                 or concepts::parse_compatible< std::decay_t< decltype(*range.begin()) > >
        :
        HttpHeaders(range.begin(), range.end()) { }

}  // namespace mt::network::http

#endif  // HTTP_HEADER_HPP
