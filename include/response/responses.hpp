// Created by Mitia Tristan on 13.01.25.

#ifndef INCLUDE_RESPONSE_RESPONSES_HPP
#define INCLUDE_RESPONSE_RESPONSES_HPP

#include "response/impl/raw_response.hpp"
#include "response/impl/http_response.hpp"

#include <variant>

namespace mt::network {
    using Response = std::variant< std::monostate, std::shared_ptr<RawResponse>, std::shared_ptr<HttpResponse> >;
}

#endif  //INCLUDE_RESPONSE_RESPONSES_HPP
