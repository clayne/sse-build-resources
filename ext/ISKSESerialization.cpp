#include "ISKSESerialization.h"

#include <skse64/GameReferences.h>

// adapted from skee64
bool ISKSESerialization::ResolveHandle(SKSESerializationInterface* a_intfc, Game::VMHandle a_handle, Game::VMHandle& a_out)
{
    if (!a_handle.IsTemporary()) 
    {
        if (!a_intfc->ResolveHandle(a_handle, std::addressof(a_out))) {
            return false;
        }
    }
    else 
    {
        auto refr = a_handle.GetFormID().Lookup<TESObjectREFR>();
        if (!refr)
            return false;

        if ((refr->flags & TESForm::kFlagIsDeleted) == TESForm::kFlagIsDeleted)
            return false;

        a_out = a_handle;
    }

    return true;
}

bool ISKSESerialization::ResolveRaceForm(
    SKSESerializationInterface* a_intfc,
    Game::FormID a_formID, 
    Game::FormID& a_out)
{
    auto form = ResolveForm<TESRace>(a_intfc, a_formID);
    if (!form) {
        return false;
    }

    a_out = form->formID;

    return true;
}

bool ISKSESerialization::ResolveFormID(
    SKSESerializationInterface* a_intfc, 
    Game::FormID a_formID, 
    Game::FormID& a_out)
{
    Game::FormID tmp;

    if (!a_formID.IsTemporary()) 
    {
        if (!a_intfc->ResolveFormId(a_formID, std::addressof(tmp))) {
            return false;
        }
    }
    else {
        tmp = a_formID;
    }

    a_out = tmp;

    return true;
}
