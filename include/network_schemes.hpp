#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_NETWORK_SCHEMES_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_NETWORK_SCHEMES_HPP

#include <string>
#include <cstdint>

namespace mt::network::schemes {

    struct NetworkScheme {
        std::string name;
        uint16_t port;
    };
    [[nodiscard]] auto schemeSupported(const std::string& scheme) -> bool;
    [[nodiscard]] auto getNetworkSchemeDefaultPort(const std::string& p_scheme_name) noexcept -> uint16_t;
    [[nodiscard]] auto getNetworkSchemeName(uint16_t p_port) noexcept -> std::string;

}  // namespace tristan::network::schemes

#endif  //NETWORK_REQUEST_HANDLER_INCLUDE_NETWORK_SCHEMES_HPP
