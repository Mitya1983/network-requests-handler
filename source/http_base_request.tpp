// Created by Mitia mt on 13.01.25.

#ifndef SOURCE_HTTP_BASE_REQUEST_TPP
#define SOURCE_HTTP_BASE_REQUEST_TPP

#include "include/network_utility.hpp"
#include "include/http/http_header_names.hpp"

#include "sockets/include/inet_socket.hpp"
#include "sockets/include/socket_error.hpp"

#include <thread>

template < class Derived > void ::mt::network::HttpRequest< Derived >::processRequest() {
    sockets::InetSocket socket;

    if (socket.error()) {
        RequestBase::setError(socket.error());
        return;
    }

    RequestBase::setStatus(Status::Processed);

    socket.setHost(uint32_t{m_url.hostIP()}, m_url.host());
    socket.setPort(m_url.port_network_byte_order());
    socket.setNonBlocking();
    auto start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    while (not socket.connected()) {
        if (m_paused.load(std::memory_order_relaxed)) {
            return;
        }
        if (m_canceled.load(std::memory_order_relaxed)) {
            return;
        }
        socket.connect(m_ssl);

        if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
            return;
        }
        if (socket.error()) {
            socket.resetError();
            std::this_thread::sleep_for(m_sleeping_interval);
        }
    }
    prepareRequest();
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
        bytes_written += static_cast< int64_t >(socket.write(begin, end));
        if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
            return;
        }
        if (socket.error()) {
            socket.resetError();
            std::this_thread::sleep_for(m_sleeping_interval);
        }
    }

    RequestBase::setStatus(Status::Reading);
    std::vector< std::byte > headers_data;
    start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    while (true) {
        if (m_paused.load(std::memory_order_relaxed)) {
            return;
        }
        if (m_canceled.load(std::memory_order_relaxed)) {
            return;
        }
        std::array delimiter{'\r', '\n', '\r', '\n'};
        auto data = socket.readUntil(delimiter);
        if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
            return;
        }
        if (const auto error = socket.error(); error && socket.error().value() != static_cast< int >(mt::sockets::Error::READ_DONE)) {
            if (not data.empty()) {
                std::copy(std::make_move_iterator(std::begin(data)), std::make_move_iterator(std::end(data)), std::back_inserter(headers_data));
            }
            socket.resetError();
            std::this_thread::sleep_for(m_sleeping_interval);
            continue;
        }
        if (not data.empty()) {
            std::copy(std::make_move_iterator(std::begin(data)), std::make_move_iterator(std::end(data)), std::back_inserter(headers_data));
        }
        m_response = std::make_shared< HttpResponse >(m_id, std::move(headers_data));
        if (m_error) {
            return;
        }
        break;
    }

    auto response = std::get< std::shared_ptr< HttpResponse > >(m_response);
    if (response->status() != http::Status::Ok) {
        RequestBase::setStatus(Status::Done);
        return;
    }

    if (auto content_length = response->headers().headerValue(mt::network::http::header_names::content_length)) {
        m_bytes_to_read = std::stoll(content_length.value());
        if (m_bytes_to_read != 0) {
            socket.resetError();
            RequestBase::setStatus(Status::Reading);
            int64_t bytes_read = 0;
            start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
            while (bytes_read < m_bytes_to_read) {
                if (m_paused.load(std::memory_order_relaxed)) {
                    return;
                }
                if (m_canceled.load(std::memory_order_relaxed)) {
                    return;
                }
                auto bytes_remain = m_bytes_to_read - bytes_read;
                uint16_t current_frame_size = m_max_frame_size < bytes_remain ? m_max_frame_size : bytes_remain;
                auto data = socket.read(current_frame_size);
                if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
                    return;
                }
                if (not data.empty()) {
                    bytes_read += std::ssize(data);
                    RequestBase::addResponseData(std::move(data));
                    if (m_error) {
                        return;
                    }
                }
                if (socket.error()) {
                    socket.resetError();
                    std::this_thread::sleep_for(m_sleeping_interval);
                }
            }
        } else {
            RequestBase::setStatus(Status::Done);
        }
    } else if (auto transfer_encoding = response->headers().headerValue(mt::network::http::header_names::transfer_encoding)) {
        if (transfer_encoding.value().find("chunked") == std::string::npos) {
            RequestBase::setStatus(Status::Done);
            return;
        }
        while (true) {
            if (m_paused.load(std::memory_order_relaxed)) {
                return;
            }
            if (m_canceled.load(std::memory_order_relaxed)) {
                return;
            }
            std::array delimiter{'\r', '\n'};
            auto chunk_size = socket.readUntil(delimiter);
            if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
                return;
            }
            if (socket.error() && socket.error().value() != static_cast< int32_t >(mt::sockets::Error::READ_DONE)) {
                socket.resetError();
                std::this_thread::sleep_for(m_sleeping_interval);
                continue;
            }
            auto pos = std::ranges::find(chunk_size, std::byte{';'});
            const int64_t bytes_to_read = std::stoll(utility::string(chunk_size.begin(), pos), nullptr, 16);
            if (bytes_to_read == 0) {
                break;
            }
            socket.resetError();
            int64_t bytes_read = 0;
            start = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
            while (bytes_read < bytes_to_read) {
                if (m_paused.load(std::memory_order_relaxed)) {
                    return;
                }
                if (m_canceled.load(std::memory_order_relaxed)) {
                    return;
                }
                auto bytes_remain = bytes_to_read - bytes_read;
                uint16_t current_frame_size = m_max_frame_size < bytes_remain ? m_max_frame_size : bytes_remain;
                auto data = socket.read(current_frame_size);
                if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
                    return;
                }
                if (not data.empty()) {
                    bytes_read += std::ssize(data);
                    RequestBase::addResponseData(std::move(data));
                    if (m_error) {
                        return;
                    }
                }
                if (socket.error()) {
                    socket.resetError();
                    std::this_thread::sleep_for(m_sleeping_interval);
                }
            }
            auto redundant_data = socket.read(2);
            if (not RequestBase::checkSocketOperationErrorAndTimeOut(socket, start)) {
                return;
            }
            if (socket.error()) {
                socket.resetError();
                std::this_thread::sleep_for(std::chrono::milliseconds(m_sleeping_interval));
            }
        }
    } else {
        RequestBase::setError(mt::network::makeError(mt::network::HttpErrors::Http_response_size_error));
        return;
    }
    RequestBase::setStatus(Status::Done);
}

template < class Derived >
mt::network::HttpRequest< Derived >::HttpRequest(Url p_url) :
    RequestBase(std::move(p_url)),
    m_request_composed(false) {
    if (not m_url.isValid() || (m_url.scheme() != "http" && m_url.scheme() != "https" && m_url.port() != "80" && m_url.port() != "443")) {
        setError(makeError(ErrorCode::Invalid_url));
        return;
    }
    m_headers.addHeader(http::Header(http::header_names::host, m_url.host()));
    if (m_url.port_local_byte_order() == 443) {
        m_ssl = true;
    }
}

template < class Derived > void mt::network::HttpRequest< Derived >::addHeader(http::Header header) {
    if (header.name.empty()) {
        return;
    }
    m_headers.addHeader(std::move(header));
}

template < class Derived > void mt::network::HttpRequest< Derived >::addParam(http::Parameter parameter) {
    if (parameter.name.empty()) {
        return;
    }
    m_params.addParameter(std::move(parameter));
}
#endif  // SOURCE_HTTP_BASE_REQUEST_TPP