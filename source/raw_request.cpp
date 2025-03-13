#include "include/request/impl/raw_request.hpp"

#include "include/response/impl/raw_response.hpp"
#include "sockets/include/tcp_socket.hpp"

#include <thread>

mt::network::RawRequest::RawRequest(Url p_url) :
    RequestBase(std::move(p_url)) { }

auto mt::network::RawRequest::requestData() -> const std::vector< std::byte >& { return m_request_data; }

void mt::network::RawRequest::setClientCertificate(std::filesystem::path p_path) {
    m_client_certificate = std::move(p_path);
}

void mt::network::RawRequest::setClientKey(std::filesystem::path p_path) {
    m_client_key = std::move(p_path);
}

void mt::network::RawRequest::processRequest() {

    std::unique_ptr<sockets::TcpSocket> socket;
    if (not m_client_certificate.empty() or not m_client_key.empty()) {
        socket = std::make_unique<sockets::TcpSocket>(m_client_certificate, m_client_key);
    } else {
        socket = std::make_unique<sockets::TcpSocket>(m_ssl);
    }

    if (const auto error = socket->error(); error) {
        RequestBase::setError(error);
        return;
    }
    RequestBase::setStatus(Status::Processed);
    socket->setDestinationHost(uint32_t{m_url.hostIP()}, m_url.host());
    socket->setDestinationPort(m_url.port_network_byte_order());
    socket->setNonBlocking();

    auto start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    while (not socket->connected()) {
        if (m_paused.load(std::memory_order_relaxed)) {
            return;
        }
        if (m_canceled.load(std::memory_order_relaxed)) {
            return;
        }
        socket->connect();

        if (not RequestBase::checkSocketOperationErrorAndTimeOut(*socket, start)) {
            return;
        }
        if (not socket->connected()) {
            socket->resetError();
            std::this_thread::sleep_for(m_sleeping_interval);
        }
    }

    int64_t bytes_written = 0;
    const int64_t bytes_to_write = std::ssize(m_request_data);
    RequestBase::setStatus(Status::Writing);
    start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    while (bytes_written < bytes_to_write) {
        if (m_paused.load(std::memory_order_relaxed)) {
            return;
        }
        if (m_canceled.load(std::memory_order_relaxed)) {
            return;
        }
        const auto bytes_remain = bytes_to_write - bytes_written;
        const uint16_t current_frame_size = m_max_frame_size < bytes_remain ? m_max_frame_size : bytes_remain;
        const auto begin = std::begin(m_request_data) + bytes_written;
        const auto end = begin + current_frame_size;
        bytes_written += static_cast< int64_t >(socket->write(begin, end));
        if (not RequestBase::checkSocketOperationErrorAndTimeOut(*socket, start)) {
            return;
        }
        if (socket->error()) {
            socket->resetError();
            std::this_thread::sleep_for(m_sleeping_interval);
        }
    }

    if (m_bytes_to_read != 0) {
        m_response = std::make_shared< RawResponse >(m_id);
        int64_t bytes_read = 0;
        start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
        while (bytes_read < m_bytes_to_read) {
            if (m_paused.load(std::memory_order_relaxed)) {
                return;
            }
            if (m_canceled.load(std::memory_order_relaxed)) {
                return;
            }
            const auto bytes_remain = m_bytes_to_read - bytes_read;
            const uint16_t current_frame_size = m_max_frame_size < bytes_remain ? m_max_frame_size : bytes_remain;
            auto data = socket->read(current_frame_size);
            if (not RequestBase::checkSocketOperationErrorAndTimeOut(*socket, start)) {
                return;
            }
            if (not data.empty()) {
                bytes_read += std::ssize(data);
                RequestBase::addResponseData(std::move(data));
                if (m_error) {
                    return;
                }
            }
            if (socket->error()) {
                socket->resetError();
                std::this_thread::sleep_for(m_sleeping_interval);
            }
        }
    }
    RequestBase::setStatus(Status::Done);
}

auto mt::network::RawRequest::_asyncProcessRequest() -> mt::ResumableCoroutine {
    std::unique_ptr<sockets::TcpSocket> socket;
    if (not m_client_certificate.empty() or not m_client_key.empty()) {
        socket = std::make_unique<sockets::TcpSocket>(m_client_certificate, m_client_key);
    } else {
        socket = std::make_unique<sockets::TcpSocket>(m_ssl);
    }

    if (const auto error = socket->error(); error) {
        RequestBase::setError(error);
        co_return;
    }
    RequestBase::setStatus(Status::Processed);
    socket->setDestinationHost(uint32_t{m_url.hostIP()}, m_url.host());
    socket->setDestinationPort(m_url.port_network_byte_order());
    socket->setNonBlocking();

    auto start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    while (not socket->connected()) {
        if (m_paused.load(std::memory_order_relaxed)) {
            co_return;
        }
        if (m_canceled.load(std::memory_order_relaxed)) {
            co_return;
        }
        socket->connect();

        if (not RequestBase::checkSocketOperationErrorAndTimeOut(*socket, start)) {
            co_return;
        }
        co_await std::suspend_always();
        socket->resetError();
    }
    m_max_frame_size = std::numeric_limits<uint8_t>::max();
    int64_t bytes_written = 0;
    const int64_t bytes_to_write = std::ssize(m_request_data);
    RequestBase::setStatus(Status::Writing);
    start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    while (bytes_written < bytes_to_write) {
        if (m_paused.load(std::memory_order_relaxed)) {
            co_return;
        }
        if (m_canceled.load(std::memory_order_relaxed)) {
            co_return;
        }
        const auto bytes_remain = bytes_to_write - bytes_written;
        const uint16_t current_frame_size = m_max_frame_size < bytes_remain ? m_max_frame_size : bytes_remain;
        const auto begin = std::begin(m_request_data) + bytes_written;
        const auto end = begin + current_frame_size;
        bytes_written += static_cast< int64_t >(socket->write(begin, end));
        if (not RequestBase::checkSocketOperationErrorAndTimeOut(*socket, start)) {
            co_return;
        }
        co_await std::suspend_always();
        socket->resetError();
    }

    if (m_bytes_to_read != 0) {
        m_response = std::make_shared< RawResponse >(m_id);
        int64_t bytes_read = 0;
        start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
        while (bytes_read < m_bytes_to_read) {
            if (m_paused.load(std::memory_order_relaxed)) {
                co_return;
            }
            if (m_canceled.load(std::memory_order_relaxed)) {
                co_return;
            }
            const auto bytes_remain = m_bytes_to_read - bytes_read;
            const uint8_t current_frame_size = m_max_frame_size < bytes_remain ? m_max_frame_size : bytes_remain;
            auto data = socket->read(current_frame_size);
            if (not RequestBase::checkSocketOperationErrorAndTimeOut(*socket, start)) {
                co_return;
            }
            if (not data.empty()) {
                bytes_read += std::ssize(data);
                RequestBase::addResponseData(std::move(data));
                if (m_error) {
                    co_return;
                }
            }
            co_await std::suspend_always();
            socket->resetError();
        }
    }
    RequestBase::setStatus(Status::Done);
}