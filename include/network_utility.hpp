#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_NETWORK_UTILITY_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_NETWORK_UTILITY_HPP

#include "include/concepts.hpp"

#include <filesystem>
#include <vector>
#include <algorithm>

namespace mt::network::utility {

    void encodeToUrlEncoding(std::vector<std::byte>& range);
    [[nodiscard]] auto mustBeEncoded(char symbol) -> bool;
    [[nodiscard]] auto urlEncodeSequence(char symbol) -> std::array< uint8_t, 3 >;
    [[nodiscard]] auto encodeUrl(const std::string& p_string_to_encode) -> std::string;
    [[nodiscard]] auto decodeUrl(const std::string& p_string_to_encode) -> std::string;

    [[nodiscard]] auto toNetworkByteOrder(uint16_t p_value) -> uint16_t;
    [[nodiscard]] auto toNetworkByteOrder(uint32_t p_value) -> uint16_t;
    [[nodiscard]] auto toHostByteOrder(uint16_t p_value) -> uint16_t;
    [[nodiscard]] auto toHostByteOrder(uint32_t p_value) -> uint32_t;
    [[nodiscard]] auto getLocalDnsIp() -> std::string;


}  // namespace mt::network::utility

#endif  //NETWORK_REQUEST_HANDLER_INCLUDE_NETWORK_UTILITY_HPP
