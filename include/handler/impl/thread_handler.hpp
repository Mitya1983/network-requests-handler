// Created by Mitia Tristan on 26.01.25.

#ifndef NETWORK_REQUEST_HANDLER_INCLUDE_HANDLER_IMPL_THREAD_HANDLER_HPP
#define NETWORK_REQUEST_HANDLER_INCLUDE_HANDLER_IMPL_THREAD_HANDLER_HPP

#include "include/request/requests.hpp"

namespace mt::network::handler::thread {

    void handle_request(Request p_request, bool p_block = false);

}

#endif //NETWORK_REQUEST_HANDLER_INCLUDE_HANDLER_IMPL_THREAD_HANDLER_HPP
