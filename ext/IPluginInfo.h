#pragma once

#include "ICommon.h"

#include <skse64/GameData.h>

#if defined(SKMP_PLUGIN_INFO_USE_SC)
#	include "StringCache.h"
using pluginInfoString_t = stl::fixed_string;
#	define PLUGIN_INFO_MAP_T std::unordered_map
#else
using pluginInfoString_t = std::string;
#	define PLUGIN_INFO_MAP_T stl::iunordered_map
#endif

struct pluginInfo_t
{
	std::uint32_t fileFlags;
	std::uint32_t modIndex;
	std::uint32_t lightIndex;
	pluginInfoString_t name;

	bool isLight;
	std::uint32_t partialIndex;

	pluginInfo_t(
		std::uint32_t a_fileFlags,
		std::uint8_t a_modIndex,
		std::uint16_t a_lightIndex,
		const char* a_name) :
		fileFlags(a_fileFlags),
		modIndex(a_modIndex), lightIndex(a_lightIndex), name(a_name)
	{
		isLight = (a_fileFlags & ModInfo::kFileFlags_Light) == ModInfo::kFileFlags_Light;
		partialIndex = !isLight ? a_modIndex : (std::uint32_t(0xFE000) | a_lightIndex);
	}

	[[nodiscard]] inline constexpr bool IsFormInMod(std::uint32_t a_formID) const noexcept
	{
		std::uint32_t modID = (a_formID & 0xFF000000) >> 24;

		if (!isLight && modID == modIndex)
			return true;

		if (isLight && modID == 0xFE && ((a_formID & 0x00FFF000) >> 12) == lightIndex)
			return true;

		return false;
	}

	[[nodiscard]] inline constexpr std::uint32_t GetPartialIndex() const noexcept
	{
		return partialIndex;
	}

	[[nodiscard]] inline constexpr bool IsLight() const noexcept
	{
		return isLight;
	}

	[[nodiscard]] inline Game::FormID GetFormID(Game::FormID a_formIDLower) const noexcept
	{
		return !isLight ?
                   modIndex << 24 | (a_formIDLower & 0xFFFFFF) :
                   0xFE000000 | (lightIndex << 12) | (a_formIDLower & 0xFFF);
	}

	[[nodiscard]] inline Game::FormID GetFormIDLower(Game::FormID a_formID) const noexcept
	{
		return isLight ? (a_formID & 0xFFF) : (a_formID & 0xFFFFFF);
	}

	template <class T>
	[[nodiscard]] inline constexpr T* LookupForm(Game::FormID a_formIDLower) const noexcept
	{
		return GetFormID(a_formIDLower).Lookup<T>();
	}
};

class IPluginInfo
{
public:
	using formPair_t = std::pair<pluginInfoString_t, Game::FormID>;

	IPluginInfo();

	bool Populate();

	[[nodiscard]] inline constexpr bool IsPopulated() const noexcept
	{
		return m_populated;
	}

	[[nodiscard]] inline constexpr const auto& GetIndexMap() const noexcept
	{
		return m_pluginIndexMap;
	}

	[[nodiscard]] inline constexpr const auto& GetLookupRef() const noexcept
	{
		return m_pluginNameMap;
	}

	[[nodiscard]] inline bool Empty() const noexcept
	{
		return m_pluginNameMap.empty();
	}

	[[nodiscard]] const pluginInfo_t* Lookup(const pluginInfoString_t& a_modName) const;
	[[nodiscard]] const pluginInfo_t* Lookup(std::uint32_t const a_modID) const;

	bool ResolveFormID(const pluginInfoString_t& a_modName, Game::FormID a_lower, Game::FormID& a_out) const;
	bool ResolveFormID(const formPair_t& a_pair, Game::FormID& a_out) const;

	template <class T, class form_type = stl::strip_type<T>>
	[[nodiscard]] form_type* LookupForm(const pluginInfoString_t& a_modName, Game::FormID a_formid) const;

	template <class T, class form_type = stl::strip_type<T>>
	[[nodiscard]] form_type* LookupForm(const formPair_t& a_form) const;

	[[nodiscard]] inline auto Size() const noexcept
	{
		return m_pluginIndexMap.size();
	}

private:
	bool m_populated;

	std::map<std::uint32_t, pluginInfo_t> m_pluginIndexMap;
	PLUGIN_INFO_MAP_T<pluginInfoString_t, pluginInfo_t&> m_pluginNameMap;
};

template <class T, class form_type>
form_type* IPluginInfo::LookupForm(
	const pluginInfoString_t& a_modName,
	Game::FormID a_formid) const
{
	if (auto mi = Lookup(a_modName))
	{
		return mi->GetFormID(a_formid).Lookup<form_type>();
	}

	return nullptr;
}

template <class T, class form_type>
form_type* IPluginInfo::LookupForm(const formPair_t& a_form) const
{
	if (auto mi = Lookup(a_form.first))
	{
		return mi->GetFormID(a_form.second).Lookup<form_type>();
	}

	return nullptr;
}

#undef PLUGIN_INFO_MAP_T