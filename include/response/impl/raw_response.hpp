// Created by Mitia Tristan on 07.01.25.
#ifndef INCLUDE_RESPONSE_IMPL_RAW_RESPONSE_HPP
#define INCLUDE_RESPONSE_IMPL_RAW_RESPONSE_HPP

#include "response/impl/private/base_response.hpp"

namespace mt::network {
    class RawResponse : public ResponseBase {
        friend class RawRequest;

    public:
        RawResponse() = delete;
        explicit RawResponse(const uint64_t p_id) : ResponseBase(p_id){}

        explicit RawResponse(const RawResponse& p_other) = delete;
        RawResponse(RawResponse&& p_other) noexcept = default;
        RawResponse& operator=(const RawResponse& p_other) = delete;
        RawResponse& operator=(RawResponse&& p_other) noexcept = default;
        void* operator new(size_t) = delete;
        ~RawResponse() = default;
    };
}  // namespace mt::network

#endif  // INCLUDE_RESPONSE_IMPL_RAW_RESPONSE_HPP