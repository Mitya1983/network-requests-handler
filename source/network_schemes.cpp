#include "include/network_schemes.hpp"

#include <array>

namespace {

    inline const std::array g_network_schemes = {
        mt::network::schemes::NetworkScheme{"http",  80 },
        mt::network::schemes::NetworkScheme{"https", 443},
        mt::network::schemes::NetworkScheme{"ftp",   20 },
        mt::network::schemes::NetworkScheme{"ssh",   22 },
        mt::network::schemes::NetworkScheme{"smtp",  25 },
        mt::network::schemes::NetworkScheme{"imap",  143},
        mt::network::schemes::NetworkScheme{"pop",   995}
    };
}  //End of anonymous namespace

auto mt::network::schemes::schemeSupported(const std::string& scheme) -> bool {
    return scheme == "http" || scheme == "https";
}

auto mt::network::schemes::getNetworkSchemeDefaultPort(const std::string& p_scheme_name) noexcept -> uint16_t {
    for (const auto& scheme: g_network_schemes) {
        if (scheme.name == p_scheme_name) {
            return scheme.port;
        }
    }
    return 0;
}

auto mt::network::schemes::getNetworkSchemeName(uint16_t p_port) noexcept -> std::string {
    for (const auto& scheme: g_network_schemes) {
        if (scheme.port == p_port) {
            return scheme.name;
        }
    }
    return {};
}
