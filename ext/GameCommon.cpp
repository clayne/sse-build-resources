#include "GameCommon.h"

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

        for (UInt32 i = 0; i < pl->highActorHandles.count; i++)
        {
            NiPointer<TESObjectREFR> ref;

            if (!pl->highActorHandles[i].LookupREFR(ref))
                continue;

            if (ref->formType != Actor::kTypeID)
                continue;

            auto actor = static_cast<Actor*>(ref.get());

            if (actor)
                a_func(actor);
        }
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