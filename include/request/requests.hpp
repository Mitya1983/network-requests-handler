// Created by Mitia Tristan on 13.01.25.

#ifndef INCLUDE_REQUEST_REQUESTS_HPP
#define INCLUDE_REQUEST_REQUESTS_HPP

#include "request/impl/raw_request.hpp"
#include "request/impl/http_request.hpp"

#include <variant>

namespace mt::network {
    using Request = std::variant< std::monostate, std::shared_ptr< RawRequest >, std::shared_ptr< GetRequest >, std::shared_ptr< PostRequest >, std::shared_ptr< PutRequest > >;
}

#endif  //INCLUDE_REQUEST_REQUESTS_HPP
