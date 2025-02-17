#include "../include/param.hpp"

#include <algorithm>

mt::network::Parameter::Parameter(std::string_view::const_iterator& p_begin, const std::string_view::const_iterator p_end) {
    bool parsingValue{false};
    while (p_begin != p_end) {
        if (*p_begin == '=') {
            parsingValue = true;
            ++p_begin;
            continue;
        }
        if (*p_begin == '&') {
            ++p_begin;
            return;
        }
        if (parsingValue) {
            value += *p_begin;
        } else {
            name += *p_begin;
        }
        ++p_begin;
    }
}

mt::network::Params::Params(std::string_view p_data) {
    auto iter = p_data.begin();
    while (iter != p_data.end()) {
        m_params.emplace_back(iter, p_data.end());
    }
}

void mt::network::Params::addParameter(Parameter&& p_parameter) { m_params.emplace_back(std::move(p_parameter)); }

auto mt::network::Params::parameterValue(const std::string_view p_parameter_name) const -> std::optional< std::string_view > {
    const auto found = std::ranges::find_if(m_params, [p_parameter_name](const Parameter& header) -> bool {
        return header.name == p_parameter_name;
    });
    if (found == m_params.cend()) {
        return std::nullopt;
    }
    return found->value;
}

auto mt::network::Params::empty() const -> bool { return m_params.empty(); }

auto mt::network::Params::begin() noexcept -> std::vector< Parameter >::iterator { return m_params.begin(); }

auto mt::network::Params::begin() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.begin(); }

auto mt::network::Params::cbegin() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.cbegin(); }

auto mt::network::Params::end() noexcept -> std::vector< Parameter >::iterator { return m_params.end(); }

auto mt::network::Params::end() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.end(); }

auto mt::network::Params::cend() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.cend(); }
