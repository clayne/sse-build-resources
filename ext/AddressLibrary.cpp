#include "AddressLibrary.h"
#include "PerfCounter.h"

#pragma warning(disable: 4073)
#pragma init_seg(lib)

IAL IAL::m_Instance;

IAL::IAL() :
    hasBadQuery(false)
{
    PerfCounter pc;

    tLoadStart = pc.Query();
    db = new VersionDb();
    isLoaded = db->Load();
    tLoadEnd = pc.Query();
}

IAL::~IAL()
{
    if (db != nullptr)
        delete db;
}

void IAL::Release()
{
    delete m_Instance.db;
    m_Instance.db = nullptr;
}
