#include "GameCommon.h"

#include <skse64/FormTraits.h>

namespace Game
{
    void AIProcessVisitActors(const std::function<void(Actor*)>& a_func)
    {
        auto player = *g_thePlayer;

        if (player)
            a_func(player);

        auto pl = Game::ProcessLists::GetSingleton();
        if (pl == nullptr)
            return;

        for (auto handle : pl->highActorHandles)
        {
            NiPointer<TESObjectREFR> ref;
            if (!handle.LookupREFR(ref))
                continue;

            auto actor = ref->As<Actor>();

            if (actor)
                a_func(actor);
        }
    }

    char GetActorSex(Actor* a_actor)
    {
        if (auto actorBase = a_actor->baseForm; actorBase) {
            if (auto npc = actorBase->As<TESNPC>(); npc) {
                return npc->GetSex();
            }
        }

        return 0;
    }

    TESRace* GetActorRace(Actor* a_actor)
    {
        auto race = a_actor->race;

        if (!race) 
        {
            if (auto actorBase = a_actor->baseForm; actorBase) {
                if (auto npc = actorBase->As<TESNPC>(); npc) {
                    race = npc->race.race;
                }
            }
        }

        return race;
    }


    static auto s_processLists = IAL::Addr<ProcessLists**>(514167);

    ProcessLists* ProcessLists::GetSingleton()
    {
        return *s_processLists;
    }

    bool ProcessLists::GuardsPursuing(Actor* a_actor)
    {
        return _GuardsPursuing(a_actor, 0x15, 0) != 0;
    }

    static auto s_BSMain = IAL::Addr<BSMain**>(516943);

    BSMain* BSMain::GetSingleton()
    {
        return *s_BSMain;
    }

    static Unk00* s_Unk00 = IAL::Addr<Unk00*>(523657);

    Unk00* Unk00::GetSingleton()
    {
        return s_Unk00;
    }
}