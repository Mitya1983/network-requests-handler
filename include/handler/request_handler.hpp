// Created by Mitia Tristan on 26.01.25.

#ifndef INCLUDE_HANDLER_REQUEST_HANDLER_HPP
#define INCLUDE_HANDLER_REQUEST_HANDLER_HPP

#include "include/request/requests.hpp"
#include "resumable-coroutine/include/resumable_coroutine.hpp"
#include "mutex/include/mutex.hpp"
#include <list>

namespace mt::network {
    void handle_request(Request p_request, bool p_block = false);

    class AsyncRequestHandler final {

    public:
        AsyncRequestHandler() = default;
        AsyncRequestHandler(const AsyncRequestHandler& p_other) = delete;
        AsyncRequestHandler(AsyncRequestHandler&& p_other) = delete;

        AsyncRequestHandler& operator=(const AsyncRequestHandler& p_other) = delete;
        AsyncRequestHandler& operator=(AsyncRequestHandler&& p_other) = delete;

        ~AsyncRequestHandler();
        void run();
        void addRequest(Request p_network_request);

        void setMaxProcessedRequestsCount(uint8_t p_count);
        void stop();

    private:

        mt::mutex::Mutex m_processed_requests_lock;

        std::list< mt::ResumableCoroutine > m_processed_requests;
        std::list< Request > m_processed_requests_pointers;

        std::thread m_execution_thread;

        uint8_t m_max_processed_requests_count{10};

        std::atomic< bool > m_working;
    };
}  // namespace mt::network

#endif  //INCLUDE_HANDLER_REQUEST_HANDLER_HPP
