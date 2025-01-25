// Created by Mitia Tristan on 13.01.25.

#ifndef INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP
#define INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP

#include "request/impl/private/base_request.hpp"

#include "http/http_param.hpp"
#include "http/http_header.hpp"

namespace mt::network {
    class HttpRequest : public RequestBase {
    public:
        HttpRequest(const HttpRequest& p_other) = delete;
        HttpRequest(HttpRequest&& p_other) noexcept = delete;
        HttpRequest& operator=(const HttpRequest& p_other) = delete;
        HttpRequest& operator=(HttpRequest&& p_other) noexcept = delete;
        ~HttpRequest() = default;

        void initResponse(std::vector< std::byte > headers_data);

    protected:
        void addHeader(http::Header header);
        void addParam(http::Parameter parameter);
        explicit HttpRequest(Url p_url);

        std::string m_request_name;
        http::HttpHeaders m_headers;
        http::HttpParams m_params;
        bool m_request_composed;
    };
}  // namespace mt::network

#endif  //INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP
