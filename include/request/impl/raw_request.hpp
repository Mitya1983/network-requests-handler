#ifndef INCLUDE_REQUEST_IMPL_RAW_REQUEST_HPP
#define INCLUDE_REQUEST_IMPL_RAW_REQUEST_HPP

#include "include/request/impl/private/base_request.hpp"

#include "resumable-coroutine/include/resumable_coroutine.hpp"

namespace mt::network {

    class RawRequest final : public RequestBase {
        friend class AsyncRequestHandler;

    public:
        explicit RawRequest(Url p_url);

        RawRequest() = delete;
        RawRequest(const RawRequest& p_other) = delete;
        RawRequest(RawRequest&& p_other) noexcept = delete;
        RawRequest& operator=(const RawRequest& p_other) = delete;
        RawRequest& operator=(RawRequest&& p_other) noexcept = delete;
        void* operator new(size_t) = delete;
        ~RawRequest() = default;

        auto requestData() -> const std::vector< std::byte >&;

        void setClientCertificate(std::filesystem::path p_path);
        void setClientKey(std::filesystem::path p_path);

        void processRequest();

    private:
        auto _asyncProcessRequest() -> mt::ResumableCoroutine;

        std::filesystem::path m_client_certificate;
        std::filesystem::path m_client_key;
    };
}  // namespace mt::network


#endif  // INCLUDE_REQUEST_IMPL_RAW_REQUEST_HPP