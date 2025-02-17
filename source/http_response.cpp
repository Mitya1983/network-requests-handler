#include "include/response/impl/http_response.hpp"
#include "include/network_error.hpp"
#include "include/network_utility.hpp"

mt::network::HttpResponse::HttpResponse(const uint64_t p_id, std::vector< std::byte > p_headers_data) :
    ResponseBase(p_id) {

    if (const std::string find_pattern{"HTTP/1.1"};
        !std::equal(p_headers_data.begin(), p_headers_data.begin() + 8, find_pattern.begin(), find_pattern.end(), [](const std::byte left, const char right) -> bool {
            return left == static_cast< std::byte >(right);
        })) {
        m_error = makeError(HttpErrors::Http_bad_response_format);
        return;
    }
    m_status = static_cast< http::Status >(std::stoi(utility::string(p_headers_data.begin() + 9, p_headers_data.begin() + 12)));
    m_response_headers = http::HttpHeaders(std::string_view{reinterpret_cast<char*>(p_headers_data.data()), p_headers_data.size()});
}

auto mt::network::HttpResponse::error() const -> std::error_code { return m_error; }

auto mt::network::HttpResponse::status() const -> http::Status { return m_status; }

auto mt::network::HttpResponse::headers() const -> const http::HttpHeaders& { return m_response_headers; }
