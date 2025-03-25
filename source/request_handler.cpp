// Created by Mitia Tristan on 27.01.25.
#include "include/handler/request_handler.hpp"
#include "include/handler/impl/thread_handler.hpp"
#include "include/request/impl/dns_request.hpp"

#include <thread>
#include <mutex>
#include <iostream>

void mt::network::handle_request(Request p_request, const bool p_block) { handler::thread::handle_request(std::move(p_request), p_block); }

mt::network::AsyncRequestHandler::~AsyncRequestHandler() {
    m_working.store(false);
    m_execution_thread.join();
    if (not m_processed_requests.empty()) {
        for (auto request: m_processed_requests_pointers) {
            std::visit(
                []< typename RequestT >(RequestT&& l_request) -> void {
                    if constexpr (not std::is_same_v< std::decay_t< RequestT >, std::monostate > and not std::is_same_v< std::decay_t< RequestT >, std::shared_ptr< DnsRequest > >) {
                        l_request->setStatus(mt::network::Status::Canceled);
                    }
                },
                request);
        }
    }
}

void mt::network::AsyncRequestHandler::run() {
    if (m_working.load()) {
        return;
    }
    m_working.store(true, std::memory_order_relaxed);
    m_execution_thread = std::thread([this]() -> void {
        uint8_t active_requests_counter{0};
        while (m_working) {
            if (m_processed_requests.empty()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            } else {
                auto active_requests_pointer_iterator = m_processed_requests_pointers.begin();
                for (auto active_requests_iterator = m_processed_requests.begin(); active_requests_iterator != m_processed_requests.end();) {
                    if (not active_requests_iterator->resume()) {
                        std::scoped_lock lock(m_processed_requests_lock);
                        //TODO: Resolve issue here. Something is happened and we get out from thread; Maybe we should catch exceptions;
                        active_requests_iterator = m_processed_requests.erase(active_requests_iterator);
                        if (active_requests_iterator == m_processed_requests.end()) {
                            active_requests_iterator = m_processed_requests.begin();
                        }
                        active_requests_pointer_iterator = m_processed_requests_pointers.erase(active_requests_pointer_iterator);
                        if (active_requests_pointer_iterator == m_processed_requests_pointers.end()) {
                            active_requests_pointer_iterator = m_processed_requests_pointers.begin();
                        }
                        --active_requests_counter;
                    } else {
                        if (active_requests_counter > m_max_processed_requests_count) {
                            active_requests_iterator = m_processed_requests.begin();
                            active_requests_pointer_iterator = m_processed_requests_pointers.begin();
                            continue;
                        }
                        ++active_requests_iterator;
                        ++active_requests_pointer_iterator;
                        ++active_requests_counter;
                    }
                }
            }
        }
    });
}

void mt::network::AsyncRequestHandler::addRequest(Request p_network_request) {
    if (not m_working) {
        std::visit(
            []< typename RequestT >(RequestT&& request) -> void {
                if constexpr (not std::is_same_v< std::decay_t< RequestT >, std::monostate >) {
                    request->setError(mt::network::makeError(mt::network::ErrorCode::Async_network_request_handler_was_not_lunched));
                }
            },
            p_network_request);
        return;
    }
    std::scoped_lock lock(m_processed_requests_lock);
    m_processed_requests_pointers.push_back(p_network_request);
    std::visit(
        [this]< typename RequestT >(RequestT&& request) -> void {
            if constexpr (not std::is_same_v< std::decay_t< RequestT >, std::monostate > and not std::is_same_v< std::decay_t< RequestT >, std::shared_ptr< DnsRequest > >) {
                m_processed_requests.emplace_back(request->_asyncProcessRequest());
            }
        },
        p_network_request);
}

void mt::network::AsyncRequestHandler::setMaxProcessedRequestsCount(const uint8_t p_count) { m_max_processed_requests_count = p_count; }

void mt::network::AsyncRequestHandler::stop() {
    m_working.store(false);
    m_execution_thread.join();
}