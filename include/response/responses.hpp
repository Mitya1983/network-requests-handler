// Created by Mitia Tristan on 13.01.25.

#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_RESPONSE_RESPONSES_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_RESPONSE_RESPONSES_HPP

#include "include/response/impl/raw_response.hpp"
#include "include/response/impl/http_response.hpp"

#include <variant>
#include <memory>

namespace mt::network {
    using Response = std::variant< std::monostate, std::shared_ptr<RawResponse>, std::shared_ptr<HttpResponse> >;
}

#endif  //NETWORK_REQUEST_HANDLER_INCLUDE_RESPONSE_RESPONSES_HPP
