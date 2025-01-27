// Created by Mitia Tristan on 26.01.25.
#include "include/handler/impl/thread_handler.hpp"

#include "include/request/impl/raw_request.hpp"
#include "sockets/include/inet_socket.hpp"

#include <thread>

namespace {

    struct Invoker {
        void operator()(std::monostate) { }

        void operator()(std::shared_ptr< mt::network::RawRequest > p_request) const {
            if (block) {
                p_request->processRequest();
            } else {
                std::thread(&mt::network::RawRequest::processRequest, std::move(p_request)).detach();
            }
        }

        void operator()(std::shared_ptr< mt::network::GetRequest > p_request) const {
            if (block) {
                p_request->processRequest();
            } else {
                std::thread(&mt::network::GetRequest::processRequest, std::move(p_request)).detach();
            }
        }

        void operator()(std::shared_ptr< mt::network::PostRequest > p_request) const {
            if (block) {
                p_request->processRequest();
            } else {
                std::thread(&mt::network::PostRequest::processRequest, std::move(p_request)).detach();
            }
        }

        void operator()(std::shared_ptr< mt::network::PutRequest > p_request) const {
            if (block) {
                p_request->processRequest();
            } else {
                std::thread(&mt::network::PutRequest::processRequest, std::move(p_request)).detach();
            }
        }

        bool block;
    };

}  // namespace

void mt::network::handler::thread::handle_request(Request p_request, const bool p_block) { std::visit< void >(Invoker{p_block}, p_request); }