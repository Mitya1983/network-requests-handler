#ifndef INCLUDE_REQUEST_IMPL_RAW_REQUEST_HPP
#define INCLUDE_REQUEST_IMPL_RAW_REQUEST_HPP

#include "request/impl/private/base_request.hpp"

namespace mt::network {

    class RawRequest final : public RequestBase {

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
        void initResponse();
    };

}  // namespace mt::network

#endif  // INCLUDE_REQUEST_IMPL_RAW_REQUEST_HPP