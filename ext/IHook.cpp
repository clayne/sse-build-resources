#include "IHook.h"

namespace Hook
{
    size_t GetAllocGranularity()
    {
        SYSTEM_INFO info;
        GetSystemInfo(&info);
        return static_cast<size_t>(info.dwAllocationGranularity);
    }
}