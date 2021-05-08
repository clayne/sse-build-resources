#pragma once

#include <immintrin.h>
#include <atomic>

// non-reentrant
class FastSpinMutex
{
    static inline constexpr std::uint64_t MAX_SPIN_CYCLES = 10000ULL;

public:
    FastSpinMutex() noexcept = default;

    [[nodiscard]] SKMP_FORCEINLINE bool try_lock() noexcept;
    void lock() noexcept;
    SKMP_FORCEINLINE void unlock() noexcept;

private:

    std::atomic_flag m_lock = ATOMIC_FLAG_INIT;
};

bool FastSpinMutex::try_lock() noexcept
{
    return m_lock.test_and_set(std::memory_order_acquire) == false;
}

void FastSpinMutex::unlock() noexcept
{
    m_lock.clear(std::memory_order_release);
}

class WCriticalSection
{
public:
    SKMP_FORCEINLINE WCriticalSection() noexcept {
        InitializeCriticalSection(&m_cs);
    }

    SKMP_FORCEINLINE ~WCriticalSection() noexcept {
        DeleteCriticalSection(&m_cs);
    }

    WCriticalSection(const WCriticalSection&) = delete;
    WCriticalSection(WCriticalSection&&) = delete;
    WCriticalSection& operator=(const WCriticalSection&) = delete;
    WCriticalSection& operator=(WCriticalSection&&) = delete;

    SKMP_FORCEINLINE void lock();
    SKMP_FORCEINLINE void unlock();
    SKMP_FORCEINLINE bool try_lock();

private:
    CRITICAL_SECTION m_cs;
};

SKMP_FORCEINLINE void WCriticalSection::lock() {
    EnterCriticalSection(&m_cs);
}

SKMP_FORCEINLINE void WCriticalSection::unlock() {
    LeaveCriticalSection(&m_cs);
}

SKMP_FORCEINLINE bool WCriticalSection::try_lock() {
    return TryEnterCriticalSection(&m_cs) == TRUE;
}


template <class T, class = std::enable_if_t<stl::is_any_base_of_v<T, FastSpinMutex, WCriticalSection>, void>>
class IScopedLock
{
public:

    IScopedLock() = delete;

    IScopedLock(const IScopedLock&) = delete;
    IScopedLock(IScopedLock&&) = delete;
    IScopedLock& operator=(const IScopedLock&) = delete;
    IScopedLock& operator=(IScopedLock&&) = delete;

    SKMP_FORCEINLINE IScopedLock(T& a_mutex) noexcept :
        m_mutex(a_mutex)
    {
        a_mutex.lock();
    }

    SKMP_FORCEINLINE ~IScopedLock() noexcept
    {
        m_mutex.unlock();
    }

private:

    T& m_mutex;
};