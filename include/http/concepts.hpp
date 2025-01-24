// Created by Mitia Tristan on 24.01.25.

#ifndef INCLUDE_HTTP_CONCEPTS_HPP
#define INCLUDE_HTTP_CONCEPTS_HPP

#include <utility>

namespace mt::network::http::concepts {
    template < typename T >
    concept convertible_to_char = requires { static_cast< char >(std::declval< T >()); };
    template < typename T >
    concept char_is_constructable = requires { char{std::declval< T >()}; };
    template < typename T >
    concept parse_compatible = convertible_to_char< T > or char_is_constructable< T >;
}  // namespace mt::network::http::concepts

#endif  //INCLUDE_HTTP_CONCEPTS_HPP
