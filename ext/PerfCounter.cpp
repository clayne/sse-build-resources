#include "PerfCounter.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

LARGE_INTEGER PerfCounter::perf_freq;
double PerfCounter::perf_freqf;

PerfCounter PerfCounter::m_Instance;

PerfCounter::PerfCounter()
{
    ::QueryPerformanceFrequency(&perf_freq);
    perf_freqf = static_cast<double>(perf_freq.QuadPart);
}
