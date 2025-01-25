#ifndef INCLUDE_REQUEST_IMPL_PRIVATE_REQUEST_BASE_HPP
#define INCLUDE_REQUEST_IMPL_PRIVATE_REQUEST_BASE_HPP

#include "response/responses.hpp"

#include "url.hpp"

#include <filesystem>
#include <vector>
#include <functional>
#include <atomic>
#include <fstream>
#include <chrono>

namespace mt::network {

    enum class Status : uint8_t {
        Waiting,
        Processed,
        Error,
        Writing,
        Reading,
        Paused,
        Resumed,
        Canceled,
        Done
    };
    enum class Priority : uint8_t {
        Low,
        Normal,
        High,
        Out_of_queue
    };

    class RequestBase {
    public:
        RequestBase(const RequestBase& p_other) = delete;
        RequestBase(RequestBase&& p_other) noexcept = delete;
        RequestBase& operator=(const RequestBase& p_other) = delete;
        RequestBase& operator=(RequestBase&& p_other) noexcept = delete;
        void* operator new(size_t) = delete;
        ~RequestBase() = default;
        void setPriority(Priority p_priority);
        void setBytesToRead(uint64_t p_bytes);
        void setResponseDelimiter(std::vector< std::byte > p_delimiter);
        void outputToFile(std::filesystem::path p_path);
        void cancel();
        void pauseProcessing();
        void continueProcessing();
        void setTimeOut(std::chrono::seconds p_timeout);
        [[nodiscard]] auto id() const noexcept -> uint64_t;
        [[nodiscard]] auto url() const noexcept -> const Url&;
        [[nodiscard]] auto errorCode() const noexcept -> std::error_code;
        [[nodiscard]] auto error() const noexcept -> bool;
        [[nodiscard]] auto status() const noexcept -> Status;
        [[nodiscard]] auto isPaused() const noexcept -> bool;
        [[nodiscard]] auto isCanceled() const noexcept -> bool;
        [[nodiscard]] auto priority() const noexcept -> Priority;
        [[nodiscard]] auto bytesToRead() const noexcept -> uint64_t;
        [[nodiscard]] auto bytesRead() const noexcept -> uint64_t;
        template < class ResponseType > [[nodiscard]] auto response() -> std::shared_ptr<ResponseType>;
        [[nodiscard]] auto response() const -> const Response&;

        void signupForBytesReadChange(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForBytesReadChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForBytesReadChange(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForBytesReadChange(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForBytesReadChange(std::function< void(uint64_t, uint64_t) >&& p_callback);
        template < class Object > void signupForBytesReadChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, uint64_t));
        template < class Object > void signupForBytesReadChange(Object* p_object, void (Object::*p_callback)(uint64_t, uint64_t));
        template < class Object > void signupForBytesReadChange(Object& p_object, void (Object::*p_callback)(uint64_t, uint64_t));
        void signupForFinished(std::function< void() >&& p_callback);
        template < class Object > void signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)());
        template < class Object > void signupForFinished(Object* p_object, void (Object::*p_callback)());
        template < class Object > void signupForFinished(Object& p_object, void (Object::*p_callback)());
        void signupForFinished(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForFinished(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForFinished(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForFinished(std::function< void(Response) >&& p_callback);
        template < class Object > void signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)(Response));
        template < class Object > void signupForFinished(Object* p_object, void (Object::*p_callback)(Response));
        template < class Object > void signupForFinished(Object& p_object, void (Object::*p_callback)(Response));
        void signupForFinished(std::function< void(uint64_t&, Response) >&& p_callback);
        template < class Object > void signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, Response));
        template < class Object > void signupForFinished(Object* p_object, void (Object::*p_callback)(uint64_t, Response));
        template < class Object > void signupForFinished(Object& p_object, void (Object::*p_callback)(uint64_t, Response));
        void signupForStatusChange(std::function< void() >&& p_callback);
        template < class Object > void signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)());
        template < class Object > void signupForStatusChange(Object* p_object, void (Object::*p_callback)());
        template < class Object > void signupForStatusChange(Object& p_object, void (Object::*p_callback)());
        void signupForStatusChange(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForStatusChange(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForStatusChange(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForStatusChange(std::function< void(Status) >&& p_callback);
        template < class Object > void signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(Status));
        template < class Object > void signupForStatusChange(Object* p_object, void (Object::*p_callback)(Status));
        template < class Object > void signupForStatusChange(Object& p_object, void (Object::*p_callback)(Status));
        void signupForStatusChange(std::function< void(uint64_t, Status) >&& p_callback);
        template < class Object > void signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, Status));
        template < class Object > void signupForStatusChange(Object* p_object, void (Object::*p_callback)(uint64_t, Status));
        template < class Object > void signupForStatusChange(Object& p_object, void (Object::*p_callback)(uint64_t, Status));
        void signupForPaused(std::function< void() >&& p_callback);
        template < class Object > void signupForPaused(std::weak_ptr< Object > p_object, void (Object::*p_callback)());
        template < class Object > void signupForPaused(Object* p_object, void (Object::*p_callback)());
        template < class Object > void signupForPaused(Object& p_object, void (Object::*p_callback)());
        void signupForPaused(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForPaused(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForPaused(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForPaused(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForResumed(std::function< void() >&& p_callback);
        template < class Object > void signupForResumed(std::weak_ptr< Object > p_object, void (Object::*p_callback)());
        template < class Object > void signupForResumed(Object* p_object, void (Object::*p_callback)());
        template < class Object > void signupForResumed(Object& p_object, void (Object::*p_callback)());
        void signupForResumed(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForResumed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForResumed(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForResumed(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForCancelled(std::function< void() >&& p_callback);
        template < class Object > void signupForCancelled(std::weak_ptr< Object > p_object, void (Object::*p_callback)());
        template < class Object > void signupForCancelled(Object* p_object, void (Object::*p_callback)());
        template < class Object > void signupForCancelled(Object& p_object, void (Object::*p_callback)());
        void signupForCancelled(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForCancelled(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForCancelled(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForCancelled(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForFailed(std::function< void() >&& p_callback);
        template < class Object > void signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)());
        template < class Object > void signupForFailed(Object* p_object, void (Object::*p_callback)());
        template < class Object > void signupForFailed(Object& p_object, void (Object::*p_callback)());
        void signupForFailed(std::function< void(uint64_t) >&& p_callback);
        template < class Object > void signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForFailed(Object* p_object, void (Object::*p_callback)(uint64_t));
        template < class Object > void signupForFailed(Object& p_object, void (Object::*p_callback)(uint64_t));
        void signupForFailed(std::function< void(std::error_code) >&& p_callback);
        template < class Object > void signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(std::error_code));
        template < class Object > void signupForFailed(Object* p_object, void (Object::*p_callback)(std::error_code));
        template < class Object > void signupForFailed(Object& p_object, void (Object::*p_callback)(std::error_code));
        void signupForFailed(std::function< void(uint64_t, std::error_code) >&& p_callback);
        template < class Object > void signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, std::error_code));
        template < class Object > void signupForFailed(Object* p_object, void (Object::*p_callback)(uint64_t, std::error_code));
        template < class Object > void signupForFailed(Object& p_object, void (Object::*p_callback)(uint64_t, std::error_code));

    protected:
        explicit RequestBase(Url p_url);
        void addResponseData(std::vector< std::byte > p_data);
        void setStatus(Status p_status);
        void setError(std::error_code p_error_code);

        Url m_url;
        std::filesystem::path m_output_path;
        std::vector< std::byte > m_delimiter;
        std::vector< std::function< void() > > m_bytes_read_callbacks;
        std::vector< std::function< void() > > m_finished_callbacks;
        std::vector< std::function< void() > > m_status_changed_callbacks;
        std::vector< std::function< void() > > m_paused_callbacks;
        std::vector< std::function< void() > > m_resumed_callbacks;
        std::vector< std::function< void() > > m_canceled_callbacks;
        std::vector< std::function< void() > > m_failed_callbacks;

        std::vector< std::byte > m_request_data;

        std::error_code m_error;

        Response m_response{std::monostate()};

        std::chrono::seconds m_timeout{30};

        uint64_t m_bytes_to_read{0};
        uint64_t m_bytes_read{0};
        uint64_t m_id{0};
        std::unique_ptr< std::ofstream > m_output_file;

        Status m_status{Status::Waiting};
        Priority m_priority{Priority::Normal};
        std::atomic_bool m_paused{false};
        std::atomic_bool m_canceled{false};
        bool m_output_to_file{false};
        bool m_ssl{false};
    private:
        void notifyWhenBytesReadChanged();
        void notifyWhenStatusChanged();
        void notifyWhenPaused();
        void notifyWhenResumed();
        void notifyWhenCanceled();
        void notifyWhenFinished();
        void notifyWhenFailed();
    };


    template < class ResponseType > auto RequestBase::response() -> std::shared_ptr<ResponseType> { return std::get< std::shared_ptr<ResponseType> >(m_response); }

    template < class Object >
    void RequestBase::signupForBytesReadChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_bytes_read_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_bytes_read);
            }
        });
    }

    template < class Object > void RequestBase::signupForBytesReadChange(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_bytes_read_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_bytes_read);
            }
        });
    }

    template < class Object > void RequestBase::signupForBytesReadChange(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_bytes_read_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_bytes_read);
        });
    }

    template < class Object >
    void RequestBase::signupForBytesReadChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, uint64_t)) {
        m_bytes_read_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id, m_bytes_read);
            }
        });
    }

    
    template < class Object >
    void RequestBase::signupForBytesReadChange(Object* p_object, void (Object::*p_callback)(uint64_t, uint64_t)) {
        m_bytes_read_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id, m_bytes_read);
            }
        });
    }

    
    template < class Object >
    void RequestBase::signupForBytesReadChange(Object& p_object, void (Object::*p_callback)(uint64_t, uint64_t)) {
        m_bytes_read_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id, m_bytes_read);
        });
    }

    template < class Object > void RequestBase::signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)()) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr);
            }
        });
    }

    template < class Object > void RequestBase::signupForFinished(Object* p_object, void (Object::*p_callback)()) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object);
            }
        });
    }

    template < class Object > void RequestBase::signupForFinished(Object& p_object, void (Object::*p_callback)()) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object > void RequestBase::signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForFinished(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForFinished(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id);
        });
    }

    template < class Object >
    void RequestBase::signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)(Response)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_response);
            }
        });
    }

    
    template < class Object >
    void RequestBase::signupForFinished(Object* p_object, void (Object::*p_callback)(Response)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_response);
            }
        });
    }

    
    template < class Object >
    void RequestBase::signupForFinished(Object& p_object, void (Object::*p_callback)(Response)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_response);
        });
    }

    template < class Object >
    void RequestBase::signupForFinished(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, Response)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id, m_response);
            }
        });
    }

    
    template < class Object >
    void RequestBase::signupForFinished(Object* p_object, void (Object::*p_callback)(uint64_t, Response)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id, m_response);
            }
        });
    }

    
    template < class Object >
    void RequestBase::signupForFinished(Object& p_object, void (Object::*p_callback)(uint64_t, Response)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id, m_response);
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)()) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object* p_object, void (Object::*p_callback)()) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object& p_object, void (Object::*p_callback)()) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object >
    void RequestBase::signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id);
        });
    }

    template < class Object >
    void RequestBase::signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(Status)) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_status);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object* p_object, void (Object::*p_callback)(Status)) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_status);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object& p_object, void (Object::*p_callback)(Status)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_status);
        });
    }

    template < class Object >
    void RequestBase::signupForStatusChange(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, Status)) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id, m_status);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object* p_object, void (Object::*p_callback)(uint64_t, Status)) {
        m_status_changed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id, m_status);
            }
        });
    }

    template < class Object > void RequestBase::signupForStatusChange(Object& p_object, void (Object::*p_callback)(uint64_t, Status)) {
        m_finished_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id, m_status);
        });
    }

    template < class Object > void RequestBase::signupForPaused(std::weak_ptr< Object > p_object, void (Object::*p_callback)()) {
        m_paused_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr);
            }
        });
    }

    template < class Object > void RequestBase::signupForPaused(Object* p_object, void (Object::*p_callback)()) {
        m_paused_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object);
            }
        });
    }

    template < class Object > void RequestBase::signupForPaused(Object& p_object, void (Object::*p_callback)()) {
        m_paused_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object > void RequestBase::signupForPaused(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_paused_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForPaused(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_paused_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForPaused(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_paused_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id);
        });
    }

    template < class Object > void RequestBase::signupForResumed(std::weak_ptr< Object > p_object, void (Object::*p_callback)()) {
        m_resumed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr);
            }
        });
    }

    template < class Object > void RequestBase::signupForResumed(Object* p_object, void (Object::*p_callback)()) {
        m_resumed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object);
            }
        });
    }

    template < class Object > void RequestBase::signupForResumed(Object& p_object, void (Object::*p_callback)()) {
        m_resumed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object > void RequestBase::signupForResumed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_resumed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForResumed(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_resumed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForResumed(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_resumed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id);
        });
    }

    template < class Object > void RequestBase::signupForCancelled(std::weak_ptr< Object > p_object, void (Object::*p_callback)()) {
        m_canceled_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr);
            }
        });
    }

    template < class Object > void RequestBase::signupForCancelled(Object* p_object, void (Object::*p_callback)()) {
        m_canceled_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object);
            }
        });
    }

    template < class Object > void RequestBase::signupForCancelled(Object& p_object, void (Object::*p_callback)()) {
        m_canceled_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object >
    void RequestBase::signupForCancelled(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_canceled_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForCancelled(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_canceled_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForCancelled(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_canceled_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id);
        });
    }

    template < class Object > void RequestBase::signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)()) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object* p_object, void (Object::*p_callback)()) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object& p_object, void (Object::*p_callback)()) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object > void RequestBase::signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object* p_object, void (Object::*p_callback)(uint64_t)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object& p_object, void (Object::*p_callback)(uint64_t)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object);
        });
    }

    template < class Object >
    void RequestBase::signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(std::error_code)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_error);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object* p_object, void (Object::*p_callback)(std::error_code)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_error);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object& p_object, void (Object::*p_callback)(std::error_code)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_error);
        });
    }

    template < class Object >
    void RequestBase::signupForFailed(std::weak_ptr< Object > p_object, void (Object::*p_callback)(uint64_t, std::error_code)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (auto object_ptr = p_object.lock(); object_ptr) {
                std::invoke(p_callback, object_ptr, m_id, m_error);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object* p_object, void (Object::*p_callback)(uint64_t, std::error_code)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            if (p_object != nullptr) {
                std::invoke(p_callback, p_object, m_id, m_error);
            }
        });
    }

    template < class Object > void RequestBase::signupForFailed(Object& p_object, void (Object::*p_callback)(uint64_t, std::error_code)) {
        m_failed_callbacks.emplace_back([this, p_object, p_callback]() -> void {
            std::invoke(p_callback, p_object, m_id, m_error);
        });
    }

    // void RequestBase::addResponseData(std::vector< std::byte > p_data) {
    //     const auto data_size = p_data.size();
    //     if (not m_output_to_file) {
    //         if (std::holds_alternative< std::monostate >(m_response)) {
    //             initResponse();
    //             std::visit(
    //                 [&p_data]< class ResponseType >(ResponseType&& response) -> void {
    //                     response->m_response_data = std::make_shared< std::vector< std::byte > >(std::move(p_data));
    //                 },
    //                 m_response);
    //         } else {
    //             std::visit(
    //                 [&p_data]< class ResponseType >(ResponseType&& response) -> void {
    //                     if (not response->m_response_data) {
    //                         response->m_response_data = std::make_shared< std::vector< std::byte > >(std::move(p_data));
    //                     } else {
    //                         std::ranges::copy(std::move(p_data), std::back_inserter(*response->m_response_data));
    //                     }
    //                 },
    //                 m_response);
    //         }
    //     } else {
    //         if (m_output_path.empty()) {
    //             setError(makeError(ErrorCode::File_path_empty));
    //             return;
    //         }
    //         if (not m_output_file) {
    //             m_output_file = std::make_unique< std::ofstream >(m_output_path, std::ios::binary);
    //         }
    //         if (not m_output_file->is_open()) {
    //             m_output_file->open(m_output_path, std::ios::ate | std::ios::binary | std::ios::app);
    //             if (not m_output_file->is_open()) {
    //                 setError(std::error_code(errno, std::system_category()));
    //                 return;
    //             }
    //         }
    //         m_output_file->write(reinterpret_cast< const char* >(p_data.data()), std::ssize(p_data));
    //     }
    //     m_bytes_read += data_size;
    //     notifyWhenBytesReadChanged();
    // }

    // void RequestBase::setStatus(const Status p_status) {
    //     switch (p_status) {
    //         using enum Status;
    //         case Waiting:
    //         case Writing:
    //         case Reading:
    //         case Processed:
    //             break;
    //         case Paused: {
    //             m_paused.store(true, std::memory_order_relaxed);
    //             notifyWhenPaused();
    //             if (m_output_file) {
    //                 if (m_output_file->is_open()) {
    //                     m_output_file->close();
    //                 }
    //                 m_output_file.reset();
    //             }
    //             break;
    //         }
    //         case Resumed: {
    //             m_paused.store(false, std::memory_order_relaxed);
    //             notifyWhenResumed();
    //             break;
    //         }
    //         case Error: {
    //             notifyWhenFailed();
    //             if (m_output_file) {
    //                 if (m_output_file->is_open()) {
    //                     m_output_file->close();
    //                 }
    //                 m_output_file.reset();
    //             }
    //             if (std::filesystem::exists(m_output_path)) {
    //                 std::filesystem::remove(m_output_path);
    //             }
    //             break;
    //         }
    //         case Canceled: {
    //             m_canceled.store(true, std::memory_order_relaxed);
    //             notifyWhenCanceled();
    //             if (m_output_file) {
    //                 if (m_output_file->is_open()) {
    //                     m_output_file->close();
    //                 }
    //                 m_output_file.reset();
    //             }
    //             if (std::filesystem::exists(m_output_path)) {
    //                 std::filesystem::remove(m_output_path);
    //             }
    //             break;
    //         }
    //         case Done: {
    //             notifyWhenFinished();
    //             if (m_output_file) {
    //                 if (m_output_file->is_open()) {
    //                     m_output_file->close();
    //                 }
    //                 m_output_file.reset();
    //             }
    //             break;
    //         }
    //     }
    //     m_status = p_status;
    //     notifyWhenStatusChanged();
    // }
    //
    // void RequestBase::setError(const std::error_code p_error_code) {
    //     m_error = p_error_code;
    //     setStatus(Status::Error);
    // }
}  // namespace mt::network

#endif  // INCLUDE_REQUEST_IMPL_PRIVATE_REQUEST_BASE_HPP