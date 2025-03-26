// Created by Mitia Tristan on 03.02.25.

#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_CONCEPTS_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_CONCEPTS_HPP
#include <type_traits>

namespace mt::network::concepts {
    template < typename Type >
    concept byte_comparable = std::is_integral_v< Type >;
    template < typename T >
    concept convertible_to_char = requires { static_cast< char >(std::declval< T >()); };
    template < typename T >
    concept char_is_constructable = requires { char{std::declval< T >()}; };
    template < typename T >
    concept parse_compatible = convertible_to_char< T > or char_is_constructable< T >;
}  // namespace mt::network::concepts

#endif  //NETWORK_REQUEST_HANDLER_INCLUDE_CONCEPTS_HPP
