#ifndef INCLUDE_REQUEST_IMPL_HTTP_REQUEST_HPP
#define INCLUDE_REQUEST_IMPL_HTTP_REQUEST_HPP

#include "include/request/impl/private/http_base_request.hpp"

namespace mt::network {

    class GetRequest final : public HttpRequest<GetRequest> {
    public:
        explicit GetRequest(Url url);
        GetRequest(const GetRequest& p_other) = delete;
        GetRequest(GetRequest&& p_other) noexcept = delete;
        GetRequest& operator=(const GetRequest& p_other) = delete;
        GetRequest& operator=(GetRequest&& p_other) noexcept = delete;
        void* operator new(size_t) = delete;
        ~GetRequest() = default;

    private:
        void prepareRequest();
    };

    class PostRequest : public HttpRequest<PostRequest> {
    public:
        explicit PostRequest(Url url);
        PostRequest(const PostRequest& p_other) = delete;
        PostRequest(PostRequest&& p_other) noexcept = delete;
        PostRequest& operator=(const PostRequest& p_other) = delete;
        PostRequest& operator=(PostRequest&& p_other) noexcept = delete;
        void* operator new(size_t) = delete;
        ~PostRequest() = default;

        void setBody(std::string p_body);


    private:
        void prepareRequest();

        std::string m_body;
    };

    class PutRequest final : public HttpRequest<PutRequest> {
    public:
        explicit PutRequest(Url url);
        PutRequest(const PutRequest& p_other) = delete;
        PutRequest(PutRequest&& p_other) noexcept = delete;
        PutRequest& operator=(const PutRequest& p_other) = delete;
        PutRequest& operator=(PutRequest&& p_other) noexcept = delete;
        void* operator new(size_t) = delete;
        ~PutRequest() = default;

        void setBody(std::string p_body);

    private:
        void prepareRequest();

        std::string m_body;
    };

}  // namespace mt::network

#endif  // INCLUDE_REQUEST_IMPL_HTTP_REQUEST_HPP
