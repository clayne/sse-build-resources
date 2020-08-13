#include "IHook.h"

namespace Hook
{
    size_t GetAllocGranularity()
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return static_cast<size_t>(info.dwAllocationGranularity);
    }

    size_t GetAlignedTrampolineSize(size_t maxSize)
    {
        auto alignTo = GetAllocGranularity();
        if (alignTo == 0) {
            return maxSize;
        }

        auto r = maxSize % alignTo;
        return r ? maxSize + (alignTo - r) : maxSize;
    }

    size_t InitBranchTrampoline(const SKSEInterface* skse, size_t a_size)
    {
        auto iface = reinterpret_cast<SKSETrampolineInterface*>(skse->QueryInterface(kInterface_Trampoline));
        if (iface != nullptr) 
        {
            auto base = iface->AllocateFromBranchPool(skse->GetPluginHandle(), a_size);
            if (base != nullptr)
            {
                g_branchTrampoline.SetBase(a_size, base);
                return a_size;
            }
        }

        size_t sz = GetAlignedTrampolineSize(a_size);
        if (g_branchTrampoline.Create(sz))
            return sz;
        else
            return 0;
    }

    size_t InitLocalTrampoline(const SKSEInterface* skse, size_t a_size)
    {
        auto iface = reinterpret_cast<SKSETrampolineInterface*>(skse->QueryInterface(kInterface_Trampoline));
        if (iface != nullptr)
        {
            auto base = iface->AllocateFromBranchPool(skse->GetPluginHandle(), a_size);
            if (base != nullptr)
            {
                g_localTrampoline.SetBase(a_size, base);
                return a_size;
            }
        }

        size_t sz = GetAlignedTrampolineSize(a_size);
        if (g_localTrampoline.Create(sz))
            return sz;
        else
            return 0;
    }
}