#ifndef HTTP_PARAM_HPP
#define HTTP_PARAM_HPP

#include "include/http/concepts.hpp"

#include <string>
#include <vector>
#include <optional>

namespace mt::network::http {

    struct Parameter {
        std::string name;
        std::string value;

        Parameter(std::string name_, std::string value_) :
            name(std::move(name_)),
            value(std::move(value_)) { }

        explicit Parameter(std::indirectly_readable auto& begin, std::indirectly_readable auto end)
            requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*end) > > or concepts::parse_compatible< std::decay_t< decltype(*begin) > >
        {
            bool parsingValue{false};
            while (begin != end) {
                char ch;
                if constexpr (concepts::convertible_to_char< std::decay_t< decltype(*begin) > >) {
                    ch = static_cast< char >(*begin);
                } else {
                    ch = char{*begin};
                }
                if (ch == '=') {
                    parsingValue = true;
                    ++begin;
                    continue;
                }
                if (ch == '&') {
                    ++begin;
                    return;
                }
                if (parsingValue) {
                    value += ch;
                } else {
                    name += ch;
                }
                ++begin;
            }
        }
    };

    class HttpParams {
    public:
        HttpParams() = default;
        explicit HttpParams(std::indirectly_readable auto begin, std::indirectly_readable auto end)
            requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*end) > > or concepts::parse_compatible< std::decay_t< decltype(*begin) > >;
        explicit HttpParams(std::ranges::input_range auto&& range)
            requires std::is_same_v< std::decay_t< decltype(*range.begin()) >, std::decay_t< decltype(*range.end()) > >
                     or concepts::parse_compatible< std::decay_t< decltype(*range.begin()) > >;
        HttpParams(const HttpParams& p_other) = default;
        HttpParams(HttpParams&& p_other) noexcept = default;
        HttpParams& operator=(const HttpParams& p_other) = default;
        HttpParams& operator=(HttpParams&& p_other) noexcept = default;
        ~HttpParams() = default;

        void addParameter(Parameter&& p_parameter);

        [[nodiscard]] auto parameterValue(const std::string& p_parameter_name) const -> std::optional< std::string >;
        [[nodiscard]] auto empty() const -> bool;
        [[nodiscard]] auto begin() noexcept -> std::vector< Parameter >::iterator;
        [[nodiscard]] auto cbegin() const noexcept -> std::vector< Parameter >::const_iterator;
        [[nodiscard]] auto end() noexcept -> std::vector< Parameter >::iterator;
        [[nodiscard]] auto cend() const noexcept -> std::vector< Parameter >::const_iterator;

    private:
        std::vector< Parameter > m_params;
    };

    HttpParams::HttpParams(std::indirectly_readable auto begin, std::indirectly_readable auto end)
        requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*end) > > or concepts::parse_compatible< std::decay_t< decltype(*begin) > >
    {
        while (begin != end) {
            m_params.emplace_back(begin, end);
        }
    }

    HttpParams::HttpParams(std::ranges::input_range auto&& range)
        requires std::is_same_v< std::decay_t< decltype(*range.begin()) >, std::decay_t< decltype(*range.end()) > >
                 or concepts::parse_compatible< std::decay_t< decltype(*range.begin()) > >
        :
        HttpParams(range.begin(), range.end()) { }

}  // namespace mt::network::http

#endif  //HTTP_PARAM_HPP
