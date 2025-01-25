#ifndef INCLUDE_RESPONSE_IMPL_PRIVATE_BASE_RESPONSE_HPP
#define INCLUDE_RESPONSE_IMPL_PRIVATE_BASE_RESPONSE_HPP

#include <vector>
#include <memory>

namespace mt::network {

    class ResponseBase {

    public:
        ResponseBase() = delete;
        ResponseBase(const ResponseBase& p_other) = delete;
        ResponseBase(ResponseBase&& p_other) noexcept = default;
        ResponseBase& operator=(const ResponseBase& p_other) = delete;
        ResponseBase& operator=(ResponseBase&& p_other) noexcept = default;
        void* operator new(size_t) = delete;
        ~ResponseBase() = default;

    protected:
        explicit ResponseBase(const uint64_t p_id) :
            m_id(p_id) { }

        [[nodiscard]] auto id() const noexcept -> uint64_t { return m_id; }

        [[nodiscard]] auto data() const -> std::vector< std::byte > { return m_response_data; }

        std::vector< std::byte > m_response_data;

        uint64_t m_id{0};
    };
}  // namespace mt::network

#endif  // INCLUDE_RESPONSE_IMPL_PRIVATE_BASE_RESPONSE_HPP