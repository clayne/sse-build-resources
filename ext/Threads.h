#pragma once

#include <immintrin.h>

// non-reentrant
class FastSpinMutex
{
    static inline constexpr std::uint64_t MAX_SPIN_CYCLES = 1000ULL;

public:
    FastSpinMutex() noexcept : m_lock(0L) {};

    FastSpinMutex(const FastSpinMutex&) = delete;
    FastSpinMutex(FastSpinMutex&&) = delete;
    FastSpinMutex& operator=(const FastSpinMutex&) = delete;
    FastSpinMutex& operator=(FastSpinMutex&&) = delete;

    [[nodiscard]] SKMP_FORCEINLINE bool try_lock() noexcept;
    void lock() noexcept;
    SKMP_FORCEINLINE void unlock() noexcept;

private:

    volatile long m_lock;
};

bool FastSpinMutex::try_lock() noexcept
{
    return InterlockedCompareExchange(std::addressof(m_lock), 1L, 0L) == 0L;
}

void FastSpinMutex::unlock() noexcept
{
    InterlockedExchange(&m_lock, 0L);
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
    CRITICAL_SECTION	m_cs;
};

SKMP_FORCEINLINE void WCriticalSection::lock() {
    EnterCriticalSection(&m_cs);
}

SKMP_FORCEINLINE void WCriticalSection::unlock() {
    LeaveCriticalSection(&m_cs);
}

SKMP_FORCEINLINE bool WCriticalSection::try_lock() {
    return TryEnterCriticalSection(&m_cs) != 0;
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