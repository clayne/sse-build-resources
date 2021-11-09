#include "AddressLibrary.h"
#include "PerfCounter.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

IAL IAL::m_Instance;

IAL::IAL() :
    m_hasBadQuery(false)
{
    PerfCounter pc;

    m_tLoadStart = pc.Query();
    m_isLoaded = m_database.Load();
    m_tLoadEnd = pc.Query();
}

void IAL::Release()
{
    m_Instance.m_database.clear();
}
