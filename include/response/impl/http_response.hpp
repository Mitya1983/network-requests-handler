#ifndef INCLUDE_RESPONSE_IMPL_HTTP_RESPONSE_HPP
#define INCLUDE_RESPONSE_IMPL_HTTP_RESPONSE_HPP

#include "response/impl/private/base_response.hpp"

#include "http/http_header.hpp"
#include "http/http_status_codes.hpp"

namespace mt::network {

    class HttpResponse final : public ResponseBase {
        friend class HttpRequest;
    public:
        HttpResponse() = delete;
        explicit HttpResponse(uint64_t p_id, std::vector< std::byte > p_headers_data);
        HttpResponse(const HttpResponse& p_other) = delete;
        HttpResponse(HttpResponse&& p_other) = default;
        HttpResponse& operator=(const HttpResponse& p_other) = delete;
        HttpResponse& operator=(HttpResponse&& p_other) = default;
        void* operator new(size_t) = delete;
        ~HttpResponse() = default;

        [[nodiscard]] auto error() const -> std::error_code;

        [[nodiscard]] auto status() const -> http::HttpStatus;

        [[nodiscard]] auto headers() const -> const http::HttpHeaders&;

    private:

        std::error_code m_error;

        http::HttpHeaders m_response_headers;

        http::HttpStatus m_status;
    };

}  // namespace mt::network

#endif  // INCLUDE_RESPONSE_IMPL_HTTP_RESPONSE_HPP
