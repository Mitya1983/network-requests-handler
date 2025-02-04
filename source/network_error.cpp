#include "include/network_error.hpp"

#include <map>
#include <string>

namespace /*anonymous*/
{

    inline const std::map< mt::network::ErrorCode, std::string > g_error_code_descriptions{
        {mt::network::ErrorCode::Offline,                                       "Network offline"                                      },
        {mt::network::ErrorCode::Invalid_url,                                   "Invalid Url"                                          },
        {mt::network::ErrorCode::Host_not_found,                                "Remote host not found"                                },
        {mt::network::ErrorCode::File_path_empty,                               "Output file path is empty"                            },
        {mt::network::ErrorCode::Destination_dir_does_not_exists,               "Destination directory does not exists"                },
        {mt::network::ErrorCode::Async_network_request_handler_lunched_twice,   "AsyncRequestHandler run() function invoked twice"     },
        {mt::network::ErrorCode::Async_network_request_handler_was_not_lunched, "AsyncRequestHandler run() function was not invoked"   },
        {mt::network::ErrorCode::Request_size_is_not_appropriate,               "Request has not either bytes to read either delimiter"},
        {mt::network::ErrorCode::Request_not_supported,                         "Request type is not supported"                        },
    };

    struct NetworkCategory final : std::error_category {
        [[nodiscard]] const char* name() const noexcept override { return "mt-network-error-category"; }

        [[nodiscard]] std::string message(int32_t ec) const override { return g_error_code_descriptions.at(static_cast< mt::network::ErrorCode >(ec)); }
    };

    inline const NetworkCategory g_network_error_category;

    inline const std::map< mt::network::UrlErrors, std::string > g_url_code_descriptions{
        {mt::network::UrlErrors::Unknown_error,      "Unknown error"                                                                                   },
        {mt::network::UrlErrors::Not_found_error,    "Host not found"                                                                                  },
        {mt::network::UrlErrors::Try_again_error,    "A temporary error occurred on an authoritative name server. Try again later"                     },
        {mt::network::UrlErrors::No_recovery_error,  "A nonrecoverable name server error occurred"                                                     },
        {mt::network::UrlErrors::No_data_error,
         "The requested name is valid but does not have an IP address. Another type of request to the name server for this domain may return an answer"},
        {mt::network::UrlErrors::Bad_host_size,      "Host size is to big"                                                                             },
        {mt::network::UrlErrors::Bad_host_format,    "Host contains not allowed characters"                                                            },
        {mt::network::UrlErrors::Bad_url_format,     "Bad url format"                                                                                  },
        {mt::network::UrlErrors::Bad_ip_format,      "Bad IP format"                                                                                   },
        {mt::network::UrlErrors::Unsupported_scheme, "Unsupported or bad scheme"                                                                       },
        {mt::network::UrlErrors::Ip_converter_error, "IP address conversion failed"                                                                    },
    };

    struct UrlErrorCategory final : std::error_category {
        [[nodiscard]] const char* name() const noexcept override { return "mt-url-error-category"; }

        [[nodiscard]] std::string message(int32_t ec) const override { return g_url_code_descriptions.at(static_cast< mt::network::UrlErrors >(ec)); }
    };

    inline const UrlErrorCategory g_url_error_category;

    inline const std::map< mt::network::DnsErrors, std::string > g_dns_code_descriptions{
        {mt::network::DnsErrors::Success,                     "Success"                                           },
        {mt::network::DnsErrors::Dns_probe_finished_Nxdomain, "Dns probe finished nxdomain"                       },
        {mt::network::DnsErrors::Server_fail,                 "Server Failure"                                    },
        {mt::network::DnsErrors::Refused,                     "Query refused"                                     },
        {mt::network::DnsErrors::Timeout,                     "Request timeout"                                   },
        {mt::network::DnsErrors::Mismatch,                    "Mismatch Between Primary and Secondary DNS Servers"},
        {mt::network::DnsErrors::Incorrect,                   "Incorrect or Missing MX Records"                   },
        {mt::network::DnsErrors::Reverse_lookup_failed,       "Reverse DNS Lookup Failures"                       },
        {mt::network::DnsErrors::Propagation_delay,           "Propagation Delays"                                },
        {mt::network::DnsErrors::Incorrect_response_size,     "Incorrect response size"                           },
        {mt::network::DnsErrors::Response_id_missmatch,       "Response id does not match with query id"          },
        {mt::network::DnsErrors::Unknown_error,               "Unknown error"                                     },
    };

    struct DnsErrorCategory final : std::error_category {
        [[nodiscard]] const char* name() const noexcept override { return "mt-dns-error-category"; }

        [[nodiscard]] std::string message(int32_t ec) const override { return g_dns_code_descriptions.at(static_cast< mt::network::DnsErrors >(ec)); }
    };

    inline const DnsErrorCategory g_dns_error_category;

    inline const std::map< mt::network::HttpErrors, std::string > g_http_code_descriptions{
        {mt::network::HttpErrors::Success,                  "Success"                                                                         },
        {mt::network::HttpErrors::Bad_http_header_format,   "Bad http header format"                                                          },
        {mt::network::HttpErrors::Http_bad_response_format, "Bad format of the received http response"                                        },
        {mt::network::HttpErrors::Http_response_size_error, "Content-length and transfer-encoding chunked are not present in response headers"},
        {mt::network::HttpErrors::Unknown_error,            "Unknown error"                                                                   },
    };

    struct HttpErrorCategory final : std::error_category {
        [[nodiscard]] const char* name() const noexcept override { return "mt-http-error-category"; }

        [[nodiscard]] std::string message(int32_t ec) const override { return g_http_code_descriptions.at(static_cast< mt::network::HttpErrors >(ec)); }
    };

    inline const HttpErrorCategory g_http_error_category;
    // inline const HttpErrorCategory g_http_error_category;
    //
    // inline const std::map< mt::network::ResponseError, std::string > g_response_code_descriptions{
    //     {mt::network::ResponseError::Http_bad_response_format, "Bad format of the received http response"                                        },
    //     {mt::network::ResponseError::Http_response_size_error, "Content-length and transfer-encoding chunked are not present in response headers"},
    // };
    //
    // struct ResponseErrorCategory final : std::error_category {
    //     [[nodiscard]] const char* name() const noexcept override;
    //
    //     [[nodiscard]] std::string message(int32_t ec) const override;
    // };
    //
    // inline const HttpErrorCategory g_http_error_category;

}  // namespace

mt::network::NetworkException::NetworkException(const ErrorCode code) { m_error_description = g_error_code_descriptions.at(code); }

mt::network::NetworkException::NetworkException(const UrlErrors code) { m_error_description = g_url_code_descriptions.at(code); }

mt::network::NetworkException::NetworkException(const HttpErrors code) { m_error_description = g_http_code_descriptions.at(code); }

// mt::network::NetworkException::NetworkException(const ResponseError code) { m_error_description = g_response_code_descriptions.at(code); }

const char* mt::network::NetworkException::what() const noexcept { return exception::what(); }

auto mt::network::makeError(const ErrorCode p_error) -> std::error_code { return {static_cast< int32_t >(p_error), g_network_error_category}; }

auto mt::network::networkErrorCategory() -> const std::error_category& { return g_network_error_category; }

auto mt::network::makeError(const UrlErrors p_error) -> std::error_code { return {static_cast< int32_t >(p_error), g_url_error_category}; }

auto mt::network::urlErrorCategory() -> const std::error_category& { return g_url_error_category; }

auto mt::network::makeError(const HttpErrors p_error) -> std::error_code { return {static_cast< int32_t >(p_error), g_http_error_category}; }

auto mt::network::httpErrorCategory() -> const std::error_category& { return g_http_error_category; }

auto mt::network::makeError(DnsErrors p_error) -> std::error_code { return {static_cast< int32_t >(p_error), g_dns_error_category}; }

auto mt::network::dnsErrorCategory() -> const std::error_category& { return g_dns_error_category; }

// auto mt::network::makeError(const ResponseError p_error) -> std::error_code { return g_response_code_descriptions.at(error); }