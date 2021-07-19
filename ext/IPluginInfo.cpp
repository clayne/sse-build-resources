#include "IPluginInfo.h"

IPluginInfo::IPluginInfo() :
    m_populated(false)
{
}

bool IPluginInfo::Populate()
{
    auto dh = DataHandler::GetSingleton();
    if (!dh)
        return false;

    m_pluginIndexMap.clear();
    m_pluginNameMap.clear();

    for (auto it = dh->modList.modInfoList.Begin(); !it.End(); ++it)
    {
        auto modInfo = it.Get();
        if (!modInfo)
            continue;

        if (!modInfo->IsActive())
            continue;

        auto r = m_pluginIndexMap.try_emplace(
            it->GetPartialIndex(),
            modInfo->fileFlags,
            modInfo->modIndex,
            modInfo->lightIndex,
            modInfo->name);

        m_pluginNameMap.try_emplace(r.first->second.name, r.first->second);
    }

    return (m_populated = true);
}

const pluginInfo_t* IPluginInfo::Lookup(const std::string& a_modName) const
{
    auto it = m_pluginNameMap.find(a_modName);
    if (it != m_pluginNameMap.end()) {
        return std::addressof(it->second);
    }
    return nullptr;
}

const pluginInfo_t* IPluginInfo::Lookup(UInt32 const a_modID) const
{
    auto it = m_pluginIndexMap.find(a_modID);
    if (it != m_pluginIndexMap.end()) {
        return std::addressof(it->second);
    }
    return nullptr;
}

bool IPluginInfo::ResolveFormID(const std::string& a_modName, Game::FormID a_lower, Game::FormID& a_out) const
{
    auto pluginInfo = Lookup(a_modName);
    if (!pluginInfo) {
        return false;
    }

    a_out = pluginInfo->GetFormID(a_lower);

    return true;
}