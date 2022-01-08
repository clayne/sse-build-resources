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
			NiPointer<Actor> actor;
			if (!handle.Lookup(actor))
				continue;

			a_func(actor);
		}
	}

	char GetActorSex(Actor* a_actor)
	{
		if (auto actorBase = a_actor->baseForm; actorBase)
		{
			if (auto npc = actorBase->As<TESNPC>(); npc)
			{
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
			if (auto actorBase = a_actor->baseForm)
			{
				if (auto npc = actorBase->As<TESNPC>())
				{
					race = npc->race.race;
				}
			}
		}

		return race;
	}

	float GetNPCWeight(TESNPC* a_npc)
	{
		if (auto templ = a_npc->GetRootTemplate())
		{
			return templ->weight;
		}

		return a_npc->weight;
	}

	float GetActorWeight(Actor* a_actor)
	{
		return a_actor->GetWeight();
	}

	TESObjectARMO* GetActorSkin(Actor* a_actor)
	{
		auto npc = a_actor->baseForm ?
                       a_actor->baseForm->As<TESNPC>() :
                       nullptr;

		if (npc)
		{
			if (npc->skinForm.skin)
				return npc->skinForm.skin;
		}

		auto actorRace = a_actor->race;
		if (actorRace)
		{
			return actorRace->skin.skin;
		}

		if (npc)
		{
			if (actorRace = npc->race.race)
			{
				return actorRace->skin.skin;
			}
		}

		return nullptr;
	}

	void AIProcessVisitActors2(
		const std::function<void(Actor*, const Game::ActorHandle&)>& a_func,
		bool a_includePlayer)
	{
		if (a_includePlayer)
		{
			if (auto player = *g_thePlayer)
			{
				a_func(player, player->GetHandle());
			}
		}

		auto pl = Game::ProcessLists::GetSingleton();
		if (pl == nullptr)
		{
			return;
		}

		for (auto handle : pl->highActorHandles)
		{
			NiPointer<Actor> actor;

			if (handle.Lookup(actor))
			{
				a_func(actor, handle);
			}
		}
	}

	namespace Debug
	{
		using notification_t = void (*)(const char*, const char*, bool);

		static const auto s_notificationImpl = IAL::Addr<notification_t>(52050, 52933);

		void Notification(const char* a_message, bool a_cancelIfQueued, const char* a_sound)
		{
			s_notificationImpl(a_message, a_sound, a_cancelIfQueued);
		}
	}

	static auto s_processLists = IAL::Addr<ProcessLists**>(514167, 400315);

	ProcessLists* ProcessLists::GetSingleton()
	{
		return *s_processLists;
	}

	bool ProcessLists::GuardsPursuing(Actor* a_actor)
	{
		return _GuardsPursuing(a_actor, 0x15, 0) != 0;
	}

	static auto s_BSMain = IAL::Addr<Main**>(516943, 403449);

	Main* Main::GetSingleton()
	{
		return *s_BSMain;
	}

	static Unk00* s_Unk00 = IAL::Addr<Unk00*>(523657, 410196);

	Unk00* Unk00::GetSingleton()
	{
		return s_Unk00;
	}
}