#ifndef NETWORK_UTILITY_HPP
#define NETWORK_UTILITY_HPP

#include <filesystem>

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
} //End of tristan::network::utility namespace

#endif //NETWORK_UTILITY_HPP
