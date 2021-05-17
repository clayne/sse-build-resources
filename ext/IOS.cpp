#include "IOS.h"

std::size_t GetAllocGranularity()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return static_cast<std::size_t>(info.dwAllocationGranularity);
}
