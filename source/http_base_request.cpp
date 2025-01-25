// Created by Mitia Tristan on 13.01.25.

#include "request/impl/private/http_base_request.hpp"

#include "http/http_header_names.hpp"

mt::network::HttpRequest::HttpRequest(Url p_url) :
    RequestBase(std::move(p_url)),
    m_request_composed(false) {
    if (not m_url.isValid() || (m_url.scheme() != "http" && m_url.scheme() != "https" && m_url.port() != "80" && m_url.port() != "443")) {
        setError(makeError(ErrorCode::Invalid_url));
        return;
    }
    m_headers.addHeader(http::Header(http::header_names::host, m_url.host()));
    if (m_url.port_network_byte_order() == 443) {
        m_ssl = true;
    }
}

void mt::network::HttpRequest::addHeader(http::Header header) {
    if (header.name.empty()) {
        return;
    }
    m_headers.addHeader(std::move(header));
}

void mt::network::HttpRequest::addParam(http::Parameter parameter) {
    if (parameter.name.empty()) {
        return;
    }
    m_params.addParameter(std::move(parameter));
}

void mt::network::HttpRequest::initResponse(std::vector< std::byte > headers_data) { m_response = std::make_shared< HttpResponse >(m_id, std::move(headers_data)); }