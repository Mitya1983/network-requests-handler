#include "include/network_utility.hpp"

#include <arpa/inet.h>

#include <cmath>
#include <random>
#include <unordered_map>

namespace {

    std::unordered_map< char, std::string > percentage_encoding = {
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

void mt::network::utility::checkFileName(std::filesystem::path& p_path) {
    uint16_t file_counter = 1;
    while (true) {
        if (std::filesystem::exists(p_path)) {
            auto file_extension = p_path.extension().string();
            if (not file_extension.empty()) {
                p_path.replace_extension("");
            }
            auto file_name = p_path.filename().string();
            if (file_counter == 1) {
                file_name += "(" + std::to_string(file_counter) + ")";
            } else {
                auto pos = file_name.find("(" + std::to_string(file_counter - 1) + ")");
                auto replace_counter = static_cast< uint16_t >(log10(file_counter) + 1);
                if (file_counter == 10 || file_counter == 100 || file_counter == 1000 || file_counter == 10000) {
                    file_name.replace(pos + 1, replace_counter - 1, std::to_string(file_counter));
                } else {
                    file_name.replace(pos + 1, replace_counter, std::to_string(file_counter));
                }
            }
            p_path.replace_filename(file_name);
            if (not file_extension.empty()) {
                p_path.replace_extension(file_extension);
            }
            ++file_counter;
        } else {
            break;
        }
    }
}

auto mt::network::utility::getUuid() -> std::string {
    std::string uuid;

    std::uniform_int_distribution< uint8_t > distribution_one(48, 57);
    std::uniform_int_distribution< uint8_t > distribution_two(97, 102);
    std::uniform_int_distribution< uint8_t > distribution_choice(1, 2);
    for (int i = 0; i < 36; ++i) {
        std::mt19937_64 generator(std::chrono::system_clock::now().time_since_epoch().count());
        uint8_t which_distribution = distribution_choice(generator);
        uint8_t uuid_value = 0;
        switch (which_distribution) {
            case 1: {
                uuid_value = distribution_one(generator);
                break;
            }
            case 2: {
                uuid_value = distribution_two(generator);
                break;
            }
            default: {
                break;
            }
        }
        uuid += static_cast< char >(uuid_value);
        if (uuid.size() == 8 || uuid.size() == 13 || uuid.size() == 18 || uuid.size() == 23) {
            uuid += '-';
        }
        if (uuid.size() == 14) {
            uuid += '4';
        }
        if (uuid.size() == 19) {
            uuid += 'a';
        }
    }

    return uuid;
}

auto mt::network::utility::encodeUrl(const std::string& p_string_to_encode) -> std::string {
    std::string result(p_string_to_encode);
    uint64_t char_to_encode = 0;
    while (true) {
        char_to_encode = result.find_first_of(" !@#$%&*()+=[]:;\',/?", char_to_encode);
        if (char_to_encode == std::string::npos) {
            break;
        }
        result.replace(char_to_encode, 1, percentage_encoding.at(result.at(char_to_encode)));
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
            if (auto iter = std::ranges::find_if(percentage_encoding,
                                                 [&encoded_char](const std::pair< char, std::string >& map_pair) -> bool {
                                                     if (map_pair.second == encoded_char) {
                                                         return true;
                                                     }
                                                     return false;
                                                 });
                iter != percentage_encoding.end()) {
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

auto mt::network::utility::string(const std::vector< std::byte >::const_iterator begin, const std::vector< std::byte >::const_iterator end) -> std::string {
    std::string result;
    while (begin != end) {
        result += static_cast<char>(*begin);
    }
    return result;
}