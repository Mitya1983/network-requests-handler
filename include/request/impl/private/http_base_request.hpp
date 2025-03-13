// Created by Mitia Tristan on 13.01.25.

#ifndef INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP
#define INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP

#include "include/request/impl/private/base_request.hpp"

#include "include/param.hpp"
#include "include/http/http_header.hpp"

#include "resumable-coroutine/include/resumable_coroutine.hpp"



namespace mt::network {
    template < class Derived > class HttpRequest : public RequestBase {
        friend class AsyncRequestHandler;
    public:
        HttpRequest(const HttpRequest& p_other) = delete;
        HttpRequest(HttpRequest&& p_other) noexcept = delete;
        HttpRequest& operator=(const HttpRequest& p_other) = delete;
        HttpRequest& operator=(HttpRequest&& p_other) noexcept = delete;
        ~HttpRequest() = default;

        void processRequest();

        void setClientCertificate(std::filesystem::path p_path);
        void setClientKey(std::filesystem::path p_path);

        void addHeader(http::Header header);
        void addParam(Parameter parameter);

        [[nodiscard]] auto param(std::string_view p_param_name) const -> std::string_view;

    protected:
        explicit HttpRequest(Url p_url);

        void _prepareRequest() { static_cast< Derived* >(this)->_prepareRequest(); }
        auto _asyncProcessRequest() -> mt::ResumableCoroutine;

        std::filesystem::path m_client_certificate;
        std::filesystem::path m_client_key;
        std::string m_request_name;
        http::HttpHeaders m_headers;
        Params m_params;
        bool m_request_composed;
    };
}  // namespace mt::network

#include "source/http_base_request.tpp"

#endif  //INCLUDE_REQUEST_IMPL_PRIVATE_HTTP_BASE_REQUEST_HPP
