#ifndef NETWORK_UTILITY_HPP
#define NETWORK_UTILITY_HPP

#include "include/concepts.hpp"

#include <filesystem>
#include <vector>
#include <algorithm>
#include <array>

namespace mt::network::utility {
    /**
     * \brief Checks if filename exist and modifies the names by adding (1, 2, 3, ..., n) just before the las dot.
     * \param p_path std::filesystem::path&
     */
    void checkFileName(std::filesystem::path& p_path);
    void encodeToUrlEncoding(std::vector<std::byte>& range);
    [[nodiscard]] auto mustBeEncoded(char symbol) -> bool;
    [[nodiscard]] auto urlEncodeSequence(char symbol) -> std::array< uint8_t, 3 >;
    /**
    void encodeUrl(const std::string& p_string_to_encode);
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
    [[nodiscard]] auto string(std::vector< std::byte >::const_iterator begin, std::vector< std::byte >::const_iterator end) -> std::string;
    [[nodiscard]] auto string_view(std::vector< std::byte >& p_data) -> std::string_view;
    [[nodiscard]] auto capitalizeHttpHeader(const std::string& source) -> std::string;
    [[nodiscard]] auto toNetworkByteOrder(uint16_t p_value) -> uint16_t;
    [[nodiscard]] auto toNetworkByteOrder(uint32_t p_value) -> uint16_t;
    [[nodiscard]] auto toHostByteOrder(uint16_t p_value) -> uint16_t;
    [[nodiscard]] auto toHostByteOrder(uint32_t p_value) -> uint32_t;
    [[nodiscard]] auto generateRandomInteger(int64_t p_lower_bound = std::numeric_limits< int64_t >::min(), int64_t p_upper_bound = std::numeric_limits< int64_t >::max())
        -> int64_t;

    [[nodiscard]] auto equal(auto p_left, auto p_right) -> bool
        requires(std::is_same_v< std::decay_t< decltype(p_left) >, std::byte > or std::is_same_v< std::decay_t< decltype(p_right) >, std::byte >)
                and (concepts::byte_comparable< std::decay_t< decltype(p_left) > > or concepts::byte_comparable< std::decay_t< decltype(p_right) > >)
    {
        using LeftType = std::decay_t< decltype(p_left) >;
        using RightType = std::decay_t< decltype(p_right) >;
        if constexpr (std::is_same_v< LeftType, RightType >) {
            return p_left == p_right;
        } else if constexpr (std::is_same_v< LeftType, std::byte >) {
            return p_left == static_cast< std::byte >(p_right);
        } else {
            return static_cast< std::byte >(p_left) == p_right;
        }
    }

    [[nodiscard]] auto less(auto p_left, auto p_right) -> bool
        requires(std::is_same_v< std::decay_t< decltype(p_left) >, std::byte > or std::is_same_v< std::decay_t< decltype(p_right) >, std::byte >)
                and (concepts::byte_comparable< std::decay_t< decltype(p_left) > > or concepts::byte_comparable< std::decay_t< decltype(p_right) > >)
    {
        using LeftType = std::decay_t< decltype(p_left) >;
        using RightType = std::decay_t< decltype(p_right) >;
        if constexpr (std::is_same_v< LeftType, RightType >) {
            return p_left < p_right;
        } else if constexpr (std::is_same_v< LeftType, std::byte >) {
            return p_left < static_cast< std::byte >(p_right);
        } else {
            return static_cast< std::byte >(p_left) < p_right;
        }
    }

    void copy(std::indirectly_readable auto begin, std::indirectly_readable auto end, std::vector< std::byte >& destination)
        requires std::is_same_v< std::decay_t< decltype(*begin) >, std::decay_t< decltype(*begin) > > and std::is_integral_v< std::decay_t< decltype(*begin) > >
    {
        while (begin != end) {
            if constexpr (constexpr auto value_size = sizeof(std::decay_t< decltype(*begin) >); value_size == 1) {
                destination.emplace_back(static_cast< std::byte >(*begin));
            } else {
                auto value = *begin;
                const auto ptr = reinterpret_cast< std::byte* >(value);
                std::copy_n(ptr, value_size, std::back_inserter(destination));
            }
            ++begin;
        }
    }
}  // namespace mt::network::utility

#endif  //NETWORK_UTILITY_HPP
