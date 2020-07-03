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
}