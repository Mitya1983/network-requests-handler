#ifndef NETWORK_ERROR_HPP
#define NETWORK_ERROR_HPP

#include <system_error>
#include <cstdint>
#include <variant>


namespace mt::network {

    enum class ErrorCode : uint8_t {
        Success,
        Async_network_request_handler_lunched_twice,
        Async_network_request_handler_was_not_lunched,
        Destination_dir_does_not_exists,
        File_path_empty,
        Host_not_found,
        Invalid_url,
        Offline,
        Request_not_supported,
        Request_size_is_not_appropriate,
    };

    enum class UrlErrors : uint8_t {
        Success,
        Bad_host_format,
        Bad_host_size,
        Bad_ip_format,
        Bad_url_format,
        Ip_converter_error,
        No_data_error,
        No_recovery_error,
        Not_found_error,
        Try_again_error,
        Unknown_error,
        Unsupported_scheme,
    };

    enum class HttpErrors: uint8_t {
        Success,
        Bad_http_header_format,
        Http_bad_response_format,
        Http_response_size_error,
        Unknown_error,
    };

    // enum class ResponseError : uint8_t {
    //     Success,
    //     Http_bad_response_format,
    //     Http_response_size_error,
    // };


    class NetworkException final : public std::exception {
    public:
        explicit NetworkException(ErrorCode code);
        explicit NetworkException(UrlErrors code);
        explicit NetworkException(HttpErrors code);
        auto what() const noexcept -> const char* override;
    private:
        std::string m_error_description;
    };

    [[nodiscard]] auto makeError(ErrorCode p_error) -> std::error_code;
    [[nodiscard]] auto networkErrorCategory() -> const std::error_category&;
    [[nodiscard]] auto makeError(UrlErrors p_error) -> std::error_code;
    [[nodiscard]] auto urlErrorCategory() -> const std::error_category&;
    [[nodiscard]] auto makeError(HttpErrors p_error) -> std::error_code;
    [[nodiscard]] auto httpErrorCategory() -> const std::error_category&;
    // [[nodiscard]] auto makeError(ResponseError error) -> std::error_code;

}  // namespace mt::network

#endif  // NETWORK_ERROR_HPP
