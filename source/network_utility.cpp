#include "include/network_utility.hpp"

#include "utility/include/utility.hpp"

#include <arpa/inet.h>

#include <cmath>
#include <random>
#include <unordered_map>
#include <fstream>
#include <array>

namespace {

    std::unordered_map< char, std::string > g_percentage_encoding = {
        {' ',  "%20"},
        {'!',  "%21"},
        {'@',  "%40"},
        {'#',  "%23"},
        {'$',  "%24"},
        {'%',  "%25"},
        {'&',  "%26"},
        {'*',  "%2A"},
        {'(',  "%28"},
        {')',  "%29"},
        {'+',  "%2B"},
        {'=',  "%3D"},
        {'[',  "%5B"},
        {']',  "%5D"},
        {':',  "%3A"},
        {';',  "%3B"},
        {'\'', "%27"},
        {',',  "%2C"},
        {'/',  "%2F"},
        {'?',  "%3F"},
    };

}  //End of unnamed namespace

void mt::network::utility::encodeToUrlEncoding(std::vector< std::byte >& range) {
    // TODO: That does not work
    auto iter = range.begin();
    while (iter != range.end()) {
        if (auto ch = static_cast< char >(*iter); mustBeEncoded(ch)) {
            auto& code = g_percentage_encoding.at(ch);
            *iter = static_cast< std::byte >(*code.data());
            const auto code_ptr = reinterpret_cast< std::byte* >(code.data() + 1);
            std::copy_n(code_ptr, 2, std::inserter(range, std::next(iter)));
            iter += 2;
        } else {
            ++iter;
        }
    }
}

auto mt::network::utility::mustBeEncoded(const char symbol) -> bool { return g_percentage_encoding.contains(symbol); }

auto mt::network::utility::urlEncodeSequence(const char symbol) -> std::array< uint8_t, 3 > {
    std::array< uint8_t, 3 > code{};
    if (g_percentage_encoding.contains(symbol)) {
        auto encoding = g_percentage_encoding.at(symbol);
        std::copy_n(encoding.begin(), 3, code.begin());
    }
    return code;
}

auto mt::network::utility::encodeUrl(const std::string& p_string_to_encode) -> std::string {
    std::string result(p_string_to_encode);
    uint64_t char_to_encode = 0;
    while (true) {
        char_to_encode = result.find_first_of(" !@#$%&*()+=[]:;\',/?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        result.replace(char_to_encode, 1, g_percentage_encoding.at(result.at(char_to_encode)));
        ++char_to_encode;
    }
    return result;
}

auto mt::network::utility::decodeUrl(const std::string& p_string_to_encode) -> std::string {
    std::string result;
    auto iterator = p_string_to_encode.begin();
    while (iterator != p_string_to_encode.end()) {
        if (*iterator == '%') {
            std::string encoded_char{iterator, iterator + 2};
            if (auto iter = std::ranges::find_if(g_percentage_encoding,
                                                 [&encoded_char](const std::pair< char, std::string >& map_pair) -> bool {
                                                     if (map_pair.second == encoded_char) {
                                                         return true;
                                                     }
                                                     return false;
                                                 });
                iter != g_percentage_encoding.end()) {
                result += iter->first;
                ++ ++iterator;
                continue;
            }
            result += *iterator;
        }
        ++iterator;
    }
    return result;
}

auto mt::network::utility::toNetworkByteOrder(const uint16_t p_value) -> uint16_t {
    if constexpr (std::endian::native == std::endian::big) {
        return p_value;
    }
    // const auto ptr = reinterpret_cast< std::byte* >(&p_value);
    // std::swap(ptr[0], ptr[1]);
    // return *reinterpret_cast< uint16_t* >(ptr);
    return mt::utility::swapBytes(p_value);
}

auto mt::network::utility::toNetworkByteOrder(const uint32_t p_value) -> uint16_t {
    if constexpr (std::endian::native == std::endian::big) {
        return p_value;
    }
    // const auto ptr = reinterpret_cast< std::byte* >(&p_value);
    // std::swap(ptr[0], ptr[3]);
    // std::swap(ptr[1], ptr[2]);
    // return *reinterpret_cast< uint32_t* >(ptr);
    return mt::utility::swapBytes(p_value);
}

auto mt::network::utility::toHostByteOrder(const uint16_t p_value) -> uint16_t {
    if constexpr (std::endian::native == std::endian::big) {
        return p_value;
    }
    // const auto ptr = reinterpret_cast< std::byte* >(&p_value);
    // std::swap(ptr[0], ptr[1]);
    // return *reinterpret_cast< uint16_t* >(ptr);
    return mt::utility::swapBytes(p_value);
}

auto mt::network::utility::toHostByteOrder(const uint32_t p_value) -> uint32_t {
    if constexpr (std::endian::native == std::endian::big) {
        return p_value;
    }
    // const auto ptr = reinterpret_cast< std::byte* >(&p_value);
    // std::swap(ptr[0], ptr[3]);
    // std::swap(ptr[1], ptr[2]);
    // return *reinterpret_cast< uint32_t* >(ptr);
    return mt::utility::swapBytes(p_value);
}

auto mt::network::utility::getLocalDnsIp() -> std::string {
    std::filesystem::path resolve_path{"/etc/resolv.conf"};
    if (not std::filesystem::exists(resolve_path)) {
        return {};
    }
    std::ifstream file{resolve_path};
    if (not file.is_open()) {
        return {};
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("nameserver") != std::string::npos) {
            line = line.substr(line.find(' ') + 1);
            break;
        }
    }
    file.close();
    return line;
}