#ifndef __MUTEX_HPP__
#define __MUTEX_HPP__

#include <mutex>
#include <thread>

namespace rtt::central {
    template <typename T, typename Locker>
    class LockedData {
        T* _data_ptr;
        Locker locker;

    public:
        /**
         * @brief Construct a new Locked Data object
         * 
         * @param data Data that should be locked and accessed
         * @param mtx Mutex to lock.
         */
        LockedData(T* data, std::mutex& mtx) noexcept
            : _data_ptr{ data }, locker{ mtx } {
        }

        LockedData(LockedData&&) = default;
        LockedData(LockedData const&) = default;
        LockedData& operator=(LockedData&&) = default;
        LockedData& operator=(LockedData const&) = default;

        ~LockedData() = default;  // just drop locker and _data_ptr

        /**
         * @brief Operator overloads for ease of access
         */
        T* operator->() {
            return get();
        }

        T& operator*() {
            return *get();
        }

        T* get() {
            return _data_ptr;
        }
    };

    template <typename T, typename Locker = std::lock_guard<std::mutex>>
    class Mutex {
        T _internal_data;
        std::mutex _internal_mutex;

    public:
        Mutex(T data) noexcept
            : _internal_data{ std::move(data) } {
        }

        Mutex() = default;
        Mutex(Mutex&&) = default;
        Mutex(Mutex const&) = default;
        Mutex& operator=(Mutex&&) = default;
        Mutex& operator=(Mutex const&) = default;

        LockedData<T, Locker> acquire() noexcept {
            return LockedData<T, Locker>{ &_internal_data, _internal_mutex };
        }
    };
}  // namespace rtt::central

#endif