#pragma once

std::size_t GetAllocGranularity();

namespace WinApi
{
    void MessageBoxError(const char* a_caption, const char* a_message);
}