#include "include/request/impl/private/base_request.hpp"

#include "include/network_error.hpp"

#include "sockets/include/inet_socket.hpp"
#include "sockets/include/socket_error.hpp"

namespace {
    std::atomic< uint64_t > g_id = 1;
}

void mt::network::RequestBase::setPriority(const Priority p_priority) { m_priority = p_priority; }

void mt::network::RequestBase::setBytesToRead(const uint64_t p_bytes) { m_bytes_to_read = p_bytes; }

void mt::network::RequestBase::setResponseDelimiter(std::vector< std::byte > p_delimiter) { m_delimiter = std::move(p_delimiter); }

void mt::network::RequestBase::outputToFile(std::filesystem::path p_path) {
    m_output_path = std::move(p_path);
    m_output_to_file = true;
}

void mt::network::RequestBase::cancel() { setStatus(Status::Canceled); }

void mt::network::RequestBase::pauseProcessing() { setStatus(Status::Paused); }

void mt::network::RequestBase::continueProcessing() { setStatus(Status::Resumed); }

void mt::network::RequestBase::setTimeOut(const std::chrono::seconds p_timeout) { m_timeout = p_timeout; }

auto mt::network::RequestBase::id() const noexcept -> uint64_t { return m_id; }

auto mt::network::RequestBase::url() const noexcept -> const Url& { return m_url; }

auto mt::network::RequestBase::errorCode() const noexcept -> std::error_code { return m_error; }

auto mt::network::RequestBase::error() const noexcept -> bool { return bool{m_error}; }

auto mt::network::RequestBase::status() const noexcept -> Status { return m_status; }

auto mt::network::RequestBase::isPaused() const noexcept -> bool { return m_paused.load(std::memory_order_relaxed); }

auto mt::network::RequestBase::isCanceled() const noexcept -> bool { return m_canceled.load(std::memory_order_relaxed); }

auto mt::network::RequestBase::priority() const noexcept -> Priority { return m_priority; }

auto mt::network::RequestBase::bytesToRead() const noexcept -> int64_t { return m_bytes_to_read; }

auto mt::network::RequestBase::bytesRead() const noexcept -> int64_t { return m_bytes_read; }

auto mt::network::RequestBase::response() const -> const Response& { return m_response; }

void mt::network::RequestBase::signupForBytesReadChange(std::function< void(uint64_t) >&& p_callback) {
    m_bytes_read_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_bytes_read);
        }
    });
}

void mt::network::RequestBase::signupForBytesReadChange(std::function< void(uint64_t, uint64_t) >&& p_callback) {
    m_bytes_read_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id, m_bytes_read);
        }
    });
}

void mt::network::RequestBase::signupForFinished(std::function< void() >&& p_callback) { m_finished_callbacks.push_back(std::move(p_callback)); }

void mt::network::RequestBase::signupForFinished(std::function< void(uint64_t) >&& p_callback) {
    m_finished_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id);
        }
    });
}

void mt::network::RequestBase::signupForFinished(std::function< void(Response) >&& p_callback) {
    m_finished_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_response);
        }
    });
}

void mt::network::RequestBase::signupForFinished(std::function< void(uint64_t&, Response) >&& p_callback) {
    m_finished_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id, m_response);
        }
    });
}

void mt::network::RequestBase::signupForStatusChange(std::function< void() >&& p_callback) {
    m_status_changed_callbacks.push_back(std::move(p_callback));
}

void mt::network::RequestBase::signupForStatusChange(std::function< void(uint64_t) >&& p_callback) {
    m_status_changed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id);
        }
    });
}

void mt::network::RequestBase::signupForStatusChange(std::function< void(Status) >&& p_callback) {
    m_status_changed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_status);
        }
    });
}

void mt::network::RequestBase::signupForStatusChange(std::function< void(uint64_t, Status) >&& p_callback) {
    m_status_changed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id, m_status);
        }
    });
}

void mt::network::RequestBase::signupForPaused(std::function< void() >&& p_callback) {
    m_paused_callbacks.push_back(std::move(p_callback));
}

void mt::network::RequestBase::signupForPaused(std::function< void(uint64_t) >&& p_callback) {
    m_paused_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id);
        }
    });
}

void mt::network::RequestBase::signupForResumed(std::function< void() >&& p_callback) {
    m_resumed_callbacks.push_back(std::move(p_callback));
}

void mt::network::RequestBase::signupForResumed(std::function< void(uint64_t) >&& p_callback) {
    m_resumed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id);
        }
    });
}

void mt::network::RequestBase::signupForCancelled(std::function< void() >&& p_callback) {
    m_canceled_callbacks.push_back(std::move(p_callback));
}

void mt::network::RequestBase::signupForCancelled(std::function< void(uint64_t) >&& p_callback) {
    m_canceled_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id);
        }
    });
}

void mt::network::RequestBase::signupForFailed(std::function< void() >&& p_callback) {
    m_failed_callbacks.push_back(std::move(p_callback));
}

void mt::network::RequestBase::signupForFailed(std::function< void(uint64_t) >&& p_callback) {
    m_failed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id);
        }
    });
}

void mt::network::RequestBase::signupForFailed(std::function< void(std::error_code) >&& p_callback) {
    m_failed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_error);
        }
    });
}

void mt::network::RequestBase::signupForFailed(std::function< void(uint64_t, std::error_code) >&& p_callback) {
    m_failed_callbacks.emplace_back([this, callback = std::move(p_callback)]() -> void {
        if (callback) {
            callback(m_id, m_error);
        }
    });
}

mt::network::RequestBase::RequestBase(Url p_url) :
    m_url(std::move(p_url)) {
    m_id = g_id.fetch_add(1);
}

void mt::network::RequestBase::addResponseData(std::vector< std::byte > p_data) {
    const auto data_size = p_data.size();
    if (not m_output_to_file) {
        if (std::holds_alternative<std::monostate>(m_response)) {
            throw std::runtime_error("Should not happened");
        }
        std::visit([&p_data]<typename ResponseType>(ResponseType&& response)->void {
            if constexpr (!std::is_same_v<std::decay_t<ResponseType>, std::monostate>) {
                std::ranges::copy(p_data, std::back_inserter(response->m_response_data));
            }
        }, m_response);
    } else {
        if (m_output_path.empty()) {
            setError(makeError(ErrorCode::File_path_empty));
            return;
        }
        if (not m_output_file) {
            m_output_file = std::make_unique< std::ofstream >(m_output_path, std::ios::binary);
        }
        if (not m_output_file->is_open()) {
            m_output_file->open(m_output_path, std::ios::ate | std::ios::binary | std::ios::app);
            if (not m_output_file->is_open()) {
                setError(std::error_code(errno, std::system_category()));
                return;
            }
        }
        m_output_file->write(reinterpret_cast< const char* >(p_data.data()), std::ssize(p_data));
    }
    m_bytes_read += data_size;
    notifyWhenBytesReadChanged();
}

void mt::network::RequestBase::setStatus(const Status p_status) {
    switch (p_status) {
        using enum Status;
        case Waiting:
        case Writing:
        case Reading:
        case Processed:
            break;
        case Paused: {
            m_paused.store(true, std::memory_order_relaxed);
            notifyWhenPaused();
            if (m_output_file) {
                if (m_output_file->is_open()) {
                    m_output_file->close();
                }
                m_output_file.reset();
            }
            break;
        }
        case Resumed: {
            m_paused.store(false, std::memory_order_relaxed);
            notifyWhenResumed();
            break;
        }
        case Error: {
            notifyWhenFailed();
            if (m_output_file) {
                if (m_output_file->is_open()) {
                    m_output_file->close();
                }
                m_output_file.reset();
            }
            if (std::filesystem::exists(m_output_path)) {
                std::filesystem::remove(m_output_path);
            }
            break;
        }
        case Canceled: {
            m_canceled.store(true, std::memory_order_relaxed);
            notifyWhenCanceled();
            if (m_output_file) {
                if (m_output_file->is_open()) {
                    m_output_file->close();
                }
                m_output_file.reset();
            }
            if (std::filesystem::exists(m_output_path)) {
                std::filesystem::remove(m_output_path);
            }
            break;
        }
        case Done: {
            notifyWhenFinished();
            if (m_output_file) {
                if (m_output_file->is_open()) {
                    m_output_file->close();
                }
                m_output_file.reset();
            }
            break;
        }
    }
    m_status = p_status;
    notifyWhenStatusChanged();
}

void mt::network::RequestBase::setError(const std::error_code p_error_code) {
    m_error = p_error_code;
    setStatus(Status::Error);
}

auto mt::network::RequestBase::checkSocketOperationErrorAndTimeOut(const sockets::InetSocket& p_socket,
                                                                   const std::chrono::time_point< std::chrono::system_clock, std::chrono::microseconds > p_time_point) -> bool {
    if (const auto error = p_socket.error(); error && error.value() != static_cast< int >(sockets::Error::CONNECT_TRY_AGAIN)
            && p_socket.error().value() != static_cast< int >(sockets::Error::CONNECT_IN_PROGRESS)
            && p_socket.error().value() != static_cast< int >(sockets::Error::CONNECT_ALREADY_IN_PROCESS)
            && p_socket.error().value() != static_cast< int >(sockets::Error::WRITE_TRY_AGAIN)
            && p_socket.error().value() != static_cast< int >(sockets::Error::READ_TRY_AGAIN)
            && p_socket.error().value() != static_cast< int >(sockets::Error::READ_DONE)) {
        setError(p_socket.error());
        return false;
            }
    const auto end = std::chrono::time_point_cast< std::chrono::microseconds >(std::chrono::system_clock::now());
    if (std::chrono::duration_cast< std::chrono::seconds >(end - p_time_point) >= m_timeout) {
        setError(sockets::makeError(sockets::Error::SOCKET_TIMED_OUT));
        return false;
    }
    return true;
}

void mt::network::RequestBase::notifyWhenBytesReadChanged() {
    for (const auto& callback: m_bytes_read_callbacks) {
        callback();
    }
}

void mt::network::RequestBase::notifyWhenStatusChanged() {
    for (const auto& callback: m_status_changed_callbacks) {
        callback();
    }
}

void mt::network::RequestBase::notifyWhenPaused() {
    for (const auto& callback: m_paused_callbacks) {
        callback();
    }
}

void mt::network::RequestBase::notifyWhenResumed() {
    for (const auto& callback: m_resumed_callbacks) {
        callback();
    }
}

void mt::network::RequestBase::notifyWhenCanceled() {
    for (const auto& callback: m_canceled_callbacks) {
        callback();
    }
}

void mt::network::RequestBase::notifyWhenFinished() {
    for (const auto& callback: m_finished_callbacks) {
        callback();
    }
}

void mt::network::RequestBase::notifyWhenFailed() {
    for (const auto& callback: m_failed_callbacks) {
        callback();
    }
}