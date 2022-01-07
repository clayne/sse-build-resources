#pragma once

#include "GameHandles.h"
#include "STLCommon.h"

#include <skse64/FormTraits.h>
#include <skse64/GameForms.h>
#include <skse64/PluginAPI.h>

class ISKSESerialization
{
public:
	[[nodiscard]] static bool ResolveHandle(
		SKSESerializationInterface* a_intfc,
		Game::VMHandle a_handle,
		Game::VMHandle& a_out);

	[[nodiscard]] static bool ResolveRaceForm(
		SKSESerializationInterface* a_intfc,
		Game::FormID a_formID,
		Game::FormID& a_out);

	[[nodiscard]] static bool ResolveFormID(
		SKSESerializationInterface* a_intfc,
		Game::FormID a_formID,
		Game::FormID& a_out);

	template <
		class T,
		class form_type = stl::strip_type<T>>
	[[nodiscard]] static constexpr form_type* ResolveForm(
		SKSESerializationInterface* a_intfc,
		Game::FormID a_formID);
};

template <
	class T,
	class form_type>
static constexpr form_type* ISKSESerialization::ResolveForm(
	SKSESerializationInterface* a_intfc,
	Game::FormID a_formID)
{
	Game::FormID tmp;

	if (!a_formID.IsTemporary())
	{
		if (!a_intfc->ResolveFormId(
				a_formID,
				std::addressof(tmp)))
		{
			return nullptr;
		}
	}
	else
	{
		tmp = a_formID;
	}

	form_type* form;

	if constexpr (std::is_same_v<form_type, TESForm>)
	{
		form = tmp.Lookup();
	}
	else
	{
		form = tmp.Lookup<form_type>();
	}

	if (!form)
	{
		return nullptr;
	}

	if (form->IsDeleted())
	{
		return nullptr;
	}

	return form;
}
