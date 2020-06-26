#include "PerfCounter.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

LARGE_INTEGER PerfCounter::perf_freq;
float PerfCounter::perf_freqf;

PerfCounter PerfCounter::m_Instance;

PerfCounter::PerfCounter()
{
    ::QueryPerformanceFrequency(&perf_freq);
    perf_freqf = static_cast<float>(perf_freq.QuadPart);
}
