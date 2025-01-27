// Created by Mitia Tristan on 27.01.25.
#include "include/handler/request_handler.hpp"
#include "include/handler/impl/thread_handler.hpp"

void mt::network::handle_request(Request p_request, const bool p_queued, const bool p_block) {
    if (not p_queued) {
        handler::thread::handle_request(std::move(p_request), p_block);
    }
}