#include "include/http/http_param.hpp"

#include <algorithm>

void mt::network::http::HttpParams::addParameter(Parameter&& p_parameter) { m_params.emplace_back(std::move(p_parameter)); }

auto mt::network::http::HttpParams::parameterValue(const std::string& p_parameter_name) const -> std::optional< std::string > {
    const auto found = std::ranges::find_if(m_params, [p_parameter_name](const Parameter& header) -> bool {
        return header.name == p_parameter_name;
    });
    if (found == m_params.cend()) {
        return std::nullopt;
    }
    return found->value;
}

auto mt::network::http::HttpParams::empty() const -> bool { return m_params.empty(); }

auto mt::network::http::HttpParams::begin() noexcept -> std::vector< Parameter >::iterator { return m_params.begin(); }

auto mt::network::http::HttpParams::begin() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.begin(); }

auto mt::network::http::HttpParams::cbegin() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.cbegin(); }

auto mt::network::http::HttpParams::end() noexcept -> std::vector< Parameter >::iterator { return m_params.end(); }

auto mt::network::http::HttpParams::end() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.end(); }

auto mt::network::http::HttpParams::cend() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.cend(); }
