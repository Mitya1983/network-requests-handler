#include "../include/response/impl/http_response.hpp"
#include "../include/network_error.hpp"

mt::network::HttpResponse::HttpResponse(const uint64_t p_id, std::vector< std::byte > p_headers_data) :
    ResponseBase(p_id) {

    if (std::string(p_headers_data.begin(), p_headers_data.begin() + 8) != "HTTP/1.1") {
        m_error = makeError(HttpErrors::Http_bad_response_format);
        return;
    }
    m_status = static_cast<http::HttpStatus>(std::stoi(std::string(p_headers_data.begin() + 9, p_headers_data.begin() + 12)));
    m_response_headers = http::HttpHeaders(std::move(p_headers_data));
}

auto mt::network::HttpResponse::error() const -> std::error_code { return m_error; }

auto mt::network::HttpResponse::status() const -> http::HttpStatus { return m_status; }

auto mt::network::HttpResponse::headers() const -> const http::HttpHeaders& { return m_response_headers; }
