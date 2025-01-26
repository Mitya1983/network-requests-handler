// Created by Mitia Tristan on 26.01.25.

#ifndef INCLUDE_HANDLER_REQUEST_HANDLER_HPP
#define INCLUDE_HANDLER_REQUEST_HANDLER_HPP

#include "include/request/requests.hpp"

namespace mt::network {
    void handle_request(Request, bool queued = true, bool block = false);
}

#endif //INCLUDE_HANDLER_REQUEST_HANDLER_HPP
