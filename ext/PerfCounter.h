#pragma once

#include <chrono>

class PerfCounter
{
public:
    __forceinline static float delta(long long tp1, long long tp2) {
        return static_cast<float>(tp2 - tp1) / perf_freqf;
    }

    __forceinline static long long delta_us(long long tp1, long long tp2) {
        return ((tp2 - tp1) * 1000000LL) / perf_freq.QuadPart;
    }

    __forceinline static long long Query() {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t.QuadPart;
    }

    __forceinline static long long T(long long tp)
    {
        return (perf_freq.QuadPart / 1000000LL) * tp;
    }

private:
    PerfCounter();

    static LARGE_INTEGER perf_freq;
    static float perf_freqf;
    static PerfCounter m_Instance;
};