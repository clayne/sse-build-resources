#pragma once

std::size_t GetAllocGranularity();

namespace WinApi
{
    SKMP_FORCEINLINE void MessageBoxError(const char *a_caption, const char* a_message)
    {
        MessageBoxA(NULL, a_message, a_caption, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
    }
}