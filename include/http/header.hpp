#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_HTTP_HEADER_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_HTTP_HEADER_HPP

#include <string>
#include <vector>
#include <optional>

namespace mt::network::http {

    struct Header {
        std::string name;
        std::string value;

        explicit Header(std::string name_, std::string value_) :
            name(std::move(name_)),
            value(std::move(value_)) { }
        explicit Header(std::string_view::const_iterator& p_begin, std::string_view::const_iterator p_end);
    };

    class HttpHeaders {
    public:
        HttpHeaders() = default;
        explicit HttpHeaders(std::string_view p_data);
        HttpHeaders(const HttpHeaders& p_other) = default;
        HttpHeaders(HttpHeaders&& p_other) noexcept = default;
        HttpHeaders& operator=(const HttpHeaders& p_other) = default;
        HttpHeaders& operator=(HttpHeaders&& p_other) noexcept = default;
        ~HttpHeaders() = default;

        void addHeader(Header p_header);

        [[nodiscard]] auto headerValue(const std::string& p_header_name) const -> std::optional< std::string >;
        [[nodiscard]] auto empty() const -> bool;
        [[nodiscard]] auto begin() noexcept -> std::vector< Header >::iterator;
        [[nodiscard]] auto begin() const noexcept -> std::vector< Header >::const_iterator;
        [[nodiscard]] auto cbegin() const noexcept -> std::vector< Header >::const_iterator;
        [[nodiscard]] auto end() noexcept -> std::vector< Header >::iterator;
        [[nodiscard]] auto end() const noexcept -> std::vector< Header >::const_iterator;
        [[nodiscard]] auto cend() const noexcept -> std::vector< Header >::const_iterator;

    private:
        std::vector< Header > m_headers;
    };
}  // namespace mt::network::http

#endif  // NETWORK_REQUEST_HANDLER_INCLUDE_HTTP_HEADER_HPP
