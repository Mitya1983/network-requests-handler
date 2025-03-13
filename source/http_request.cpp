#include "include/request/impl/http_request.hpp"
#include "include/http/http_header_names.hpp"
#include "include/network_utility.hpp"

#include <thread>

mt::network::GetRequest::GetRequest(Url url) :
    HttpRequest(std::move(url)) {
    m_request_name = "GET ";
}

void mt::network::GetRequest::_prepareRequest() {
    if (not m_request_composed) {
        utility::copy(m_request_name.begin(), m_request_name.end(), m_request_data);
        if (m_url.path().empty() || m_url.path().at(0) != '/') {
            m_request_data.push_back(static_cast< std::byte >('/'));
        }
        utility::copy(m_url.path().begin(), m_url.path().end(), m_request_data);
        if (not m_params.empty()) {
            m_request_data.push_back(static_cast< std::byte >('?'));
            int32_t param_count = 0;
            for (const auto& param: m_params) {
                if (param_count > 0) {
                    m_request_data.push_back(static_cast< std::byte >('&'));
                }
                utility::copy(param.name.begin(), param.name.end(), m_request_data);
                if (!param.value.empty()) {
                    m_request_data.push_back(static_cast< std::byte >('='));
                    auto encodedValue = utility::encodeUrl(param.value);
                    utility::copy(encodedValue.begin(), encodedValue.end(), m_request_data);
                }
                ++param_count;
            }
        }
        if (const auto& l_query = m_url.query(); not l_query.empty() ) {
            if (not m_params.empty()) {
                m_request_data.push_back(static_cast< std::byte >('&'));
            } else {
                m_request_data.push_back(static_cast< std::byte >('?'));
            }
            utility::copy(l_query.begin(), l_query.end(), m_request_data);
        }
        std::string to_insert = " HTTP/1.1\r\n";
        utility::copy(to_insert.begin(), to_insert.end(), m_request_data);
        if (!m_headers.empty()) {
            for (const auto& header: m_headers) {
                utility::copy(header.name.begin(), header.name.end(), m_request_data);
                m_request_data.push_back(static_cast< std::byte >(':'));
                auto encodedValue = utility::encodeUrl(header.value);
                utility::copy(encodedValue.begin(), encodedValue.end(), m_request_data);
                m_request_data.push_back(static_cast< std::byte >('\r'));
                m_request_data.push_back(static_cast< std::byte >('\n'));
            }
        }
        m_request_data.push_back(static_cast< std::byte >('\r'));
        m_request_data.push_back(static_cast< std::byte >('\n'));
        m_request_data.shrink_to_fit();
        m_request_composed = true;
    }
}

mt::network::PostRequest::PostRequest(Url url) :
    HttpRequest(std::move(url)) {
    m_request_name = "POST ";
}

void mt::network::PostRequest::setBody(std::string p_body) { m_body = std::move(p_body); }

void mt::network::PostRequest::_prepareRequest() {
    if (not m_request_composed) {
        if (m_body.empty() and not m_params.empty()) {
            int32_t param_count = 0;
            for (const auto& param: m_params) {
                if (param_count > 0) {
                    m_body += '&';
                }
                m_body += param.name;
                m_body += '=';
                if (auto content_type = m_headers.headerValue(http::header_names::content_type); content_type && content_type.value() == "application/x-www-form-urlencoded") {
                    m_body += utility::encodeUrl(param.value);
                } else if (content_type && content_type.value() == "multipart/form-data") {
                    //NOTE: To be developed in following versions
                } else {
                    m_body += param.value;
                }
                ++param_count;
            }
        }
        m_headers.addHeader(http::Header(http::header_names::content_length, std::to_string(m_body.size())));
        utility::copy(m_request_name.begin(), m_request_name.end(), m_request_data);
        if (m_url.path().empty() || m_url.path().at(0) != '/') {
            m_request_data.push_back(static_cast< std::byte >('/'));
        }
        utility::copy(m_url.path().begin(), m_url.path().end(), m_request_data);
        std::string to_insert = " HTTP/1.1\r\n";
        utility::copy(to_insert.begin(), to_insert.end(), m_request_data);

        if (!m_headers.empty()) {
            for (const auto& header: m_headers) {
                utility::copy(header.name.begin(), header.name.end(), m_request_data);
                m_request_data.push_back(static_cast< std::byte >(':'));
                utility::copy(header.value.begin(), header.value.end(), m_request_data);
                m_request_data.push_back(static_cast< std::byte >('\r'));
                m_request_data.push_back(static_cast< std::byte >('\n'));
            }
        }

        m_request_data.push_back(static_cast< std::byte >('\r'));
        m_request_data.push_back(static_cast< std::byte >('\n'));

        if (not m_body.empty()) {
            utility::copy(m_body.begin(), m_body.end(), m_request_data);
        }
        m_request_data.shrink_to_fit();
        m_request_composed = true;
    }
}

mt::network::PutRequest::PutRequest(Url url) :
    HttpRequest(std::move(url)) {
    m_request_name = "PUT ";
}

void mt::network::PutRequest::setBody(std::string p_body) {
    m_body = std::move(p_body);
}

void mt::network::PutRequest::_prepareRequest() {
    if (not m_request_composed) {
        if (m_body.empty() and not m_params.empty()) {
            int32_t param_count = 0;
            for (const auto& param: m_params) {
                if (param_count > 0) {
                    m_body += '&';
                }
                m_body += param.name;
                m_body += '=';
                if (auto content_type = m_headers.headerValue(http::header_names::content_type); content_type && content_type.value() == "application/x-www-form-urlencoded") {
                    m_body += utility::encodeUrl(param.value);
                } else if (content_type && content_type.value() == "multipart/form-data") {
                    //NOTE: To be developed in following versions
                } else {
                    m_body += param.value;
                }
                ++param_count;
            }
        }
        m_headers.addHeader(http::Header(http::header_names::content_length, std::to_string(m_body.size())));
        utility::copy(m_request_name.begin(), m_request_name.end(), m_request_data);
        if (m_url.path().empty() || m_url.path().at(0) != '/') {
            m_request_data.push_back(static_cast< std::byte >('/'));
        }
        utility::copy(m_url.path().begin(), m_url.path().end(), m_request_data);
        std::string to_insert = " HTTP/1.1\r\n";
        utility::copy(to_insert.begin(), to_insert.end(), m_request_data);

        if (!m_headers.empty()) {
            for (const auto& header: m_headers) {
                utility::copy(header.name.begin(), header.name.end(), m_request_data);
                m_request_data.push_back(static_cast< std::byte >(':'));
                utility::copy(header.value.begin(), header.value.end(), m_request_data);
                m_request_data.push_back(static_cast< std::byte >('\r'));
                m_request_data.push_back(static_cast< std::byte >('\n'));
            }
        }

        m_request_data.push_back(static_cast< std::byte >('\r'));
        m_request_data.push_back(static_cast< std::byte >('\n'));

        if (not m_body.empty()) {
            utility::copy(m_body.begin(), m_body.end(), m_request_data);
        }
        m_request_data.shrink_to_fit();
        m_request_composed = true;
    }
}
