// Created by Mitia Tristan on 03.02.25.

#ifndef INCLUDE_CONCEPTS_HPP
#define INCLUDE_CONCEPTS_HPP
#include <type_traits>

namespace mt::network::concepts {
    template < typename Type >
    concept byte_comparable = std::is_integral_v< Type >;
}

#endif  //INCLUDE_CONCEPTS_HPP
