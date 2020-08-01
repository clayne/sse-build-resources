#pragma once

#include <chrono>

class PerfCounter
{
public:
    inline static float delta(long long tp1, long long tp2) {
        return static_cast<float>(static_cast<double>(tp2 - tp1) / perf_freqf);
    }

    inline static long long delta_us(long long tp1, long long tp2) {
        return ((tp2 - tp1) * 1000000LL) / perf_freq.QuadPart;
    }

    inline static long long Query() {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t.QuadPart;
    }

    inline static long long T(long long tp)
    {
        return (perf_freq.QuadPart / 1000000LL) * tp;
    }

private:
    PerfCounter();

    static LARGE_INTEGER perf_freq;
    static double perf_freqf;
    static PerfCounter m_Instance;
};

class PerfTimer
{
public:
    PerfTimer()
    {
    }

    inline void Start()
    {
        m_tStart = PerfCounter::Query();
    }

    inline float Stop()
    {
        return PerfCounter::delta(m_tStart, PerfCounter::Query());
    }
private:
    long long m_tStart;
};