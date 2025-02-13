// Created by Mitia Tristan on 10.02.25.

#include "include/url/url_param.hpp"

#include <algorithm>

void mt::network::url::UrlParams::addParameter(Parameter&& p_parameter) { m_params.emplace_back(std::move(p_parameter)); }

auto mt::network::url::UrlParams::parameterValue(const std::string& p_parameter_name) const -> std::optional< std::string > {
    const auto found = std::ranges::find_if(m_params, [p_parameter_name](const Parameter& header) -> bool {
        return header.name == p_parameter_name;
    });
    if (found == m_params.cend()) {
        return std::nullopt;
    }
    return found->value;
}

auto mt::network::url::UrlParams::empty() const -> bool { return m_params.empty(); }

auto mt::network::url::UrlParams::begin() noexcept -> std::vector< Parameter >::iterator { return m_params.begin(); }

auto mt::network::url::UrlParams::begin() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.begin(); }

auto mt::network::url::UrlParams::cbegin() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.cbegin(); }

auto mt::network::url::UrlParams::end() noexcept -> std::vector< Parameter >::iterator { return m_params.end(); }

auto mt::network::url::UrlParams::end() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.end(); }

auto mt::network::url::UrlParams::cend() const noexcept -> std::vector< Parameter >::const_iterator { return m_params.cend(); }