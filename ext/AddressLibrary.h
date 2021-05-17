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

    SKMP_FORCEINLINE static float GetLoadTime() {
        return IPerfCounter::delta(
            m_Instance.tLoadStart, m_Instance.tLoadEnd);
    }

    SKMP_FORCEINLINE static long long GetLoadStart() {
        return m_Instance.tLoadStart;
    }

    SKMP_FORCEINLINE static long long GetLoadEnd() {
        return m_Instance.tLoadEnd;
    }

    SKMP_FORCEINLINE static bool HasBadQuery() {
        return m_Instance.hasBadQuery;
    }

    template <typename T>
    SKMP_FORCEINLINE static T Addr(unsigned long long id)
    {
        auto r = reinterpret_cast<T>(m_Instance.db->FindAddressById(id));
        if (!r) {
            m_Instance.hasBadQuery = true;
        }
        return r;
    }

    SKMP_FORCEINLINE static uintptr_t Addr(unsigned long long id, ptrdiff_t offset)
    {
        void* addr = m_Instance.db->FindAddressById(id);
        if (addr == NULL) {
            m_Instance.hasBadQuery = true;
            return uintptr_t(0);
        }
        return reinterpret_cast<uintptr_t>(addr) + offset;
    }

    template <typename T>
    SKMP_FORCEINLINE static T Addr(unsigned long long id, ptrdiff_t offset)
    {
        return reinterpret_cast<T>(Addr(id, offset));
    }

    SKMP_FORCEINLINE static bool Offset(unsigned long long id, uintptr_t& result)
    {
        unsigned long long r;
        if (!m_Instance.db->FindOffsetById(id, r)) {
            m_Instance.hasBadQuery = true;
            return false;
        }
        result = static_cast<uintptr_t>(r);
        return true;
    }

    SKMP_FORCEINLINE static uintptr_t Offset(unsigned long long id)
    {
        unsigned long long r;
        if (!m_Instance.db->FindOffsetById(id, r)) {
            m_Instance.hasBadQuery = true;
            return uintptr_t(0);
        }
        return static_cast<uintptr_t>(r);
    }


    template <class T>
    class Address
    {
    public:
        Address() = delete;
        Address(Address&) = delete;
        Address& operator=(Address&) = delete;

        SKMP_FORCEINLINE Address(unsigned long long a_id) :
            m_offset(IAL::Addr<BlockConversionType*>(a_id))
        {
        }
        
        SKMP_FORCEINLINE Address(unsigned long long a_id, ptrdiff_t a_offset) :
            m_offset(IAL::Addr<BlockConversionType*>(a_id, a_offset))
        {
        }

        SKMP_FORCEINLINE operator T()
        {
            return reinterpret_cast <T>(m_offset);
        }

        SKMP_FORCEINLINE std::uintptr_t GetUIntPtr() const
        {
            return reinterpret_cast<std::uintptr_t>(m_offset);
        }

    private:

        struct BlockConversionType { };
        BlockConversionType* m_offset;
    };

private:
    IAL();
    ~IAL() noexcept;

    bool isLoaded;
    bool hasBadQuery;
    long long tLoadStart, tLoadEnd;

    VersionDb *db;

    static IAL m_Instance;
};
