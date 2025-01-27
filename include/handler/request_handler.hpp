// Created by Mitia Tristan on 26.01.25.

#ifndef INCLUDE_HANDLER_REQUEST_HANDLER_HPP
#define INCLUDE_HANDLER_REQUEST_HANDLER_HPP

#include "include/request/requests.hpp"

namespace mt::network {
    void handle_request(Request p_request, bool p_queued = true, bool p_block = false);
}

#endif //INCLUDE_HANDLER_REQUEST_HANDLER_HPP
