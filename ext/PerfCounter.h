#pragma once

#include <profileapi.h>

class PerfCounter
{
public:

    SKMP_FORCEINLINE PerfCounter()
    {
        ::QueryPerformanceFrequency(&perf_freq);
        perf_freqf = static_cast<double>(perf_freq.QuadPart);
    }

    SKMP_FORCEINLINE long long Query() {
        LARGE_INTEGER t;
        QueryPerformanceCounter(&t);
        return t.QuadPart;
    }

    template <class T = float>
    SKMP_FORCEINLINE float delta(long long tp1, long long tp2) {
        return static_cast<T>(static_cast<double>(tp2 - tp1) / perf_freqf);
    }

    SKMP_FORCEINLINE long long delta_us(long long tp1, long long tp2) {
        return ((tp2 - tp1) * 1000000LL) / perf_freq.QuadPart;
    }

    SKMP_FORCEINLINE long long T(long long tp)
    {
        return (perf_freq.QuadPart / 1000000LL) * tp;
    }

private:
    LARGE_INTEGER perf_freq;
    double perf_freqf;
};


class IPerfCounter
{
public:

    SKMP_FORCEINLINE static long long Query() {
        return m_Instance.Query();
    }

    SKMP_FORCEINLINE static float delta(long long tp1, long long tp2) {
        return m_Instance.delta(tp1, tp2);
    }

    SKMP_FORCEINLINE static long long delta_us(long long tp1, long long tp2) {
        return m_Instance.delta_us(tp1, tp2);
    }

    SKMP_FORCEINLINE static long long T(long long tp)
    {
        return m_Instance.T(tp);
    }

private:

    static PerfCounter m_Instance;
};

class PerfTimer
{
public:
    PerfTimer()
    {
    }

    SKMP_FORCEINLINE void Start()
    {
        m_tStart = IPerfCounter::Query();
    }

    SKMP_FORCEINLINE float Stop()
    {
        return IPerfCounter::delta(m_tStart, IPerfCounter::Query());
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
        m_tIntervalBegin(IPerfCounter::Query()),
        m_tCounter(0), m_tLast(0)
        , m_tStart(0)
    {
    }

    SKMP_FORCEINLINE void Begin()
    {
        m_tStart = IPerfCounter::Query();
    }

    SKMP_FORCEINLINE void End()
    {
        End(m_tLast);
    }

    SKMP_FORCEINLINE bool End(long long& a_out)
    {
        auto tEnd = IPerfCounter::Query();

        m_tAccum += IPerfCounter::delta_us(m_tStart, tEnd);
        m_tCounter++;

        m_tInterval = IPerfCounter::delta_us(m_tIntervalBegin, tEnd);
        if (m_tInterval >= m_interval)
        {
            if (m_tCounter > 0)
            {
                a_out = m_tAccum / m_tCounter;

                m_tCounter = 0;
                m_tAccum = 0;
                m_tIntervalBegin = tEnd;
            }
            else // overflow
            {
                a_out = 0;
                Reset();
            }

            return true;
        }

        return false;
    }

    SKMP_FORCEINLINE void SetInterval(long long a_interval)
    {
        m_interval = a_interval;
    }

    SKMP_FORCEINLINE void Reset()
    {
        m_tIntervalBegin = IPerfCounter::Query();
        m_tAccum = 0;
        m_tCounter = 0;
        m_tLast = 0;
    }

    SKMP_FORCEINLINE long long GetIntervalTime() const {
        return m_tInterval;
    }

    SKMP_FORCEINLINE long long GetTime() const {
        return m_tLast;
    }

private:
    long long m_interval;
    long long m_tStart;
    long long m_tIntervalBegin;
    long long m_tCounter;
    long long m_tAccum;

    long long m_tInterval;

    long long m_tLast;
};