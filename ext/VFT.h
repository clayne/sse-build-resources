#pragma once

namespace VTable
{
    SKMP_FORCEINLINE std::uintptr_t* Get(void* p) noexcept
    {
        return reinterpret_cast<std::uintptr_t*>(
            reinterpret_cast<std::uintptr_t*>(p)[0]);
    }

    /*template <typename T>
    SKMP_FORCEINLINE T GetProc(void* p, std::uint32_t offset) noexcept
    {
        return reinterpret_cast<T>(GetVT(p)[offset]);
    }*/

    template <class T>
    bool Detour(
        void* cl,
        std::ptrdiff_t offset,
        void* newp,
        T* oldp) noexcept
    {
        auto addr = reinterpret_cast<LPVOID>(&Get(cl)[offset]);

        DWORD oldProtect;
        if (VirtualProtect(addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE) {
            return false;
        }

        *oldp = reinterpret_cast<T>(
            InterlockedExchangePointer(
                reinterpret_cast<volatile PVOID*>(addr), newp));

        ASSERT(VirtualProtect(addr, sizeof(void*), oldProtect, &oldProtect) == TRUE);

        return true;
    }

    template <class T>
    bool Detour2(
        std::uintptr_t cl,
        std::ptrdiff_t offset,
        T* newp,
        T** oldp = nullptr) noexcept
    {
        auto addr = reinterpret_cast<LPVOID>(cl + (offset * sizeof(std::uintptr_t)));

        DWORD oldProtect;
        if (VirtualProtect(addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE) {
            return false;
        }

        auto old = reinterpret_cast<T*>(
                InterlockedExchangePointer(
                    reinterpret_cast<volatile PVOID*>(addr), static_cast<PVOID>(newp)));

        if (oldp) {
            *oldp = old;
        }

        ASSERT(VirtualProtect(addr, sizeof(void*), oldProtect, &oldProtect) == TRUE);

        return true;
    }


    template <typename T>
    bool InterlockedExchangeIfNotEqual(
        volatile PVOID* addr,
        void* newp,
        T* oldp)
    {
        PVOID initial;
        do
        {
            initial = *addr;
            if (initial == newp) {
                return false;
            }
        } while (InterlockedCompareExchangePointer(addr, newp, initial) != initial);

        *oldp = reinterpret_cast<T>(initial);

        return true;
    }

    template <typename T>
    bool Detour_s(
        void* cl,
        std::ptrdiff_t offset,
        void* newp,
        T* oldp) noexcept
    {
        auto addr = reinterpret_cast<LPVOID>(&Get(cl)[offset]);

        DWORD oldProtect;
        if (VirtualProtect(addr, sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect) == FALSE) {
            return false;
        }

        bool r = InterlockedExchangeIfNotEqual(
            reinterpret_cast<volatile PVOID*>(addr),
            newp, oldp);

        ASSERT(VirtualProtect(addr, sizeof(void*), oldProtect, &oldProtect) == TRUE);

        return r;
    }
}