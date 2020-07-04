#pragma once

#include "PerfCounter.h"
#include "versiondb.h"

class IAL
{
public:
    static void Release();
    static bool IsLoaded() {
        return m_Instance.isLoaded;
    }

    static float GetLoadTime() {
        return PerfCounter::delta(
            m_Instance.tLoadStart, m_Instance.tLoadEnd);
    }

    static long long GetLoadStart() {
        return m_Instance.tLoadStart;
    }

    static long long GetLoadEnd() {
        return m_Instance.tLoadEnd;
    }

    static bool HasBadQuery() {
        return m_Instance.hasBadQuery;
    }

    template <typename T>
    __forceinline static T Addr(unsigned long long id)
    {
        T r = reinterpret_cast<T>(m_Instance.db.FindAddressById(id));
        if (!r) {
            m_Instance.hasBadQuery = true;
        }
        return r;
    }

    __forceinline static uintptr_t Addr(unsigned long long id, uintptr_t offset)
    {
        void* addr = m_Instance.db.FindAddressById(id);
        if (addr == NULL) {
            m_Instance.hasBadQuery = true;
            return uintptr_t(0);
        }
        return reinterpret_cast<uintptr_t>(addr) + offset;
    }

    template <typename T>
    __forceinline static T Addr(unsigned long long id, uintptr_t offset)
    {
        return reinterpret_cast<T>(Addr(id, offset));
    }

    __forceinline static bool Offset(unsigned long long id, uintptr_t& result)
    {
        unsigned long long r;
        if (!m_Instance.db.FindOffsetById(id, r)) {
            m_Instance.hasBadQuery = true;
            return false;
        }
        result = static_cast<uintptr_t>(r);
        return true;
    }

    __forceinline static uintptr_t Offset(unsigned long long id)
    {
        unsigned long long r;
        if (!m_Instance.db.FindOffsetById(id, r)) {
            m_Instance.hasBadQuery = true;
            return uintptr_t(0);
        }
        return static_cast<uintptr_t>(r);
    }

private:
    IAL();

    bool isLoaded;
    bool hasBadQuery;
    long long tLoadStart, tLoadEnd;

    VersionDb db;

    static IAL m_Instance;
};
