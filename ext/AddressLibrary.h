#pragma once

#include "PerfCounter.h"
#include "versiondb.h"

#include <memory>

class IAL
{
public:


    static void Release();
    static bool IsLoaded() {
        return m_Instance.m_isLoaded;
    }

    inline static float GetLoadTime() {
        return IPerfCounter::delta(
            m_Instance.m_tLoadStart, m_Instance.m_tLoadEnd);
    }

    inline static long long GetLoadStart() {
        return m_Instance.m_tLoadStart;
    }

    inline static long long GetLoadEnd() {
        return m_Instance.m_tLoadEnd;
    }

    inline static bool HasBadQuery() {
        return m_Instance.m_hasBadQuery;
    }

    inline static std::size_t Size() {
        return m_Instance.m_database.GetOffsetMap().size();
    }

    template <typename T>
    static T Addr(unsigned long long id)
    {
        auto r = reinterpret_cast<T>(m_Instance.m_database.FindAddressById(id));
        if (!r) {
            m_Instance.m_hasBadQuery = true;
        }
        return r;
    }

    inline static uintptr_t Addr(unsigned long long id, ptrdiff_t offset)
    {
        void* addr = m_Instance.m_database.FindAddressById(id);
        if (addr == nullptr) {
            m_Instance.m_hasBadQuery = true;
            return uintptr_t(0);
        }
        return reinterpret_cast<uintptr_t>(addr) + offset;
    }

    template <typename T>
    inline static T Addr(unsigned long long id, std::ptrdiff_t offset)
    {
        return reinterpret_cast<T>(Addr(id, offset));
    }

    inline static bool Offset(unsigned long long id, std::uintptr_t& result)
    {
        unsigned long long r;
        if (!m_Instance.m_database.FindOffsetById(id, r)) {
            m_Instance.m_hasBadQuery = true;
            return false;
        }
        result = static_cast<std::uintptr_t>(r);
        return true;
    }

    inline static uintptr_t Offset(unsigned long long id)
    {
        unsigned long long r;
        if (!m_Instance.m_database.FindOffsetById(id, r)) {
            m_Instance.m_hasBadQuery = true;
            return std::uintptr_t(0);
        }
        return static_cast<std::uintptr_t>(r);
    }


    template <class T>
    class Address
    {
    public:
        Address() = delete;
        Address(Address&) = delete;
        Address& operator=(Address&) = delete;

        Address(unsigned long long a_id) :
            m_offset(IAL::Addr<BlockConversionType*>(a_id))
        {
        }

        Address(unsigned long long a_id, std::ptrdiff_t a_offset) :
            m_offset(IAL::Addr<BlockConversionType*>(a_id, a_offset))
        {
        }

        inline operator T() const
        {
            return reinterpret_cast<T>(const_cast<BlockConversionType*>(m_offset));
        }

        inline std::uintptr_t GetUIntPtr() const
        {
            return reinterpret_cast<std::uintptr_t>(m_offset);
        }

    private:

        struct BlockConversionType { };
        BlockConversionType* m_offset;
    };

private:
    IAL();
    ~IAL() = default;

    bool m_isLoaded;
    bool m_hasBadQuery;
    long long m_tLoadStart;
    long long m_tLoadEnd;

    VersionDb m_database;

    static IAL m_Instance;
};
