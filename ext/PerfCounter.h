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

class PerfTimerInt
{
public:
    PerfTimerInt(long long a_interval) :
        m_interval(a_interval),
        m_tAccum(0),
        m_tIntervalBegin(PerfCounter::Query()),
        m_tCounter(0)
    {
    }

    inline void Begin()
    {
        m_tStart = PerfCounter::Query();
    }

    inline bool End(long long& a_out)
    {
        auto tEnd = PerfCounter::Query();
        m_tAccum += PerfCounter::delta_us(m_tStart, tEnd);
        m_tCounter++;

        if (PerfCounter::delta_us(m_tIntervalBegin, tEnd) >= m_interval) {
            a_out = m_tAccum / m_tCounter;
            m_tCounter = 0;
            m_tAccum = 0;
            m_tIntervalBegin = tEnd;
            return true;
        }

        return false;
    }

    inline void SetInterval(long long a_interval)
    {
        m_interval = a_interval;
    }

    inline void Reset()
    {
        m_tIntervalBegin = PerfCounter::Query();
        m_tAccum = 0;
        m_tCounter = 0;
    }

private:
    long long m_interval;
    long long m_tStart;
    long long m_tIntervalBegin;
    long long m_tCounter;
    long long m_tAccum;
};
