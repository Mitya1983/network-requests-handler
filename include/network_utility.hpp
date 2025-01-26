#ifndef NETWORK_UTILITY_HPP
#define NETWORK_UTILITY_HPP

#include <filesystem>
#include <vector>
#include <algorithm>
namespace mt::network::utility{
    /**
     * \brief Checks if filename exist and modifies the names by adding (1, 2, 3, ..., n) just before the las dot.
     * \param p_path std::filesystem::path&
     */
    void checkFileName(std::filesystem::path& p_path);
    /**
     * \brief Generates UUID Version 4.
     * \return UUID as a std::string.
     */
    [[nodiscard]] auto getUuid() -> std::string;
    /**
     * \brief Encodes provided string to URL percentage encoding.
     * \param p_string_to_encode const std::string&
     * \return URL encoded std::string
     */
    [[nodiscard]] auto encodeUrl(const std::string& p_string_to_encode) -> std::string;
    [[nodiscard]] auto decodeUrl(const std::string& p_string_to_encode) -> std::string;
    [[nodiscard]] auto string(std::vector<std::byte>::const_iterator begin, std::vector<std::byte>::const_iterator end) -> std::string;

    void copy(std::indirectly_readable auto begin, std::indirectly_readable auto end, std::vector<std::byte>& destination)
    requires std::is_same_v<std::decay_t<decltype(*begin)>, std::decay_t<decltype(*begin)>> and std::is_integral_v<std::decay_t<decltype(*begin)>>
    {
        while (begin != end) {
            if constexpr (constexpr auto value_size = sizeof(std::decay_t< decltype(*begin) >); value_size == 1) {
                destination.emplace_back(static_cast<std::byte>(*begin));
            } else {
                auto value = *begin;
                const auto ptr = reinterpret_cast<std::byte*>(value);
                std::copy_n(ptr, value_size, std::back_inserter(destination));
            }
            ++begin;
        }
    }
} //End of tristan::network::utility namespace

#endif //NETWORK_UTILITY_HPP
