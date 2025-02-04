// Created by Mitia Tristan on 13.01.25.

#ifndef INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP
#define INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP

#include "include/request/impl/private/base_request.hpp"

#include "include/http/http_param.hpp"
#include "include/http/http_header.hpp"

namespace mt::network {
    template < class Derived > class HttpRequest : public RequestBase {
    public:
        HttpRequest(const HttpRequest& p_other) = delete;
        HttpRequest(HttpRequest&& p_other) noexcept = delete;
        HttpRequest& operator=(const HttpRequest& p_other) = delete;
        HttpRequest& operator=(HttpRequest&& p_other) noexcept = delete;
        ~HttpRequest() = default;

        void processRequest();

        void addHeader(http::Header header);
        void addParam(http::Parameter parameter);
    protected:
        explicit HttpRequest(Url p_url);


        void prepareRequest() { static_cast< Derived* >(this)->prepareRequest(); }

        std::string m_request_name;
        http::HttpHeaders m_headers;
        http::HttpParams m_params;
        bool m_request_composed;
    };
}  // namespace mt::network

#include "source/http_base_request.tpp"

#endif  //INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP
