#include "ITaskPool.h"

#include <skse64/GameReferences.h>

ITaskPool ITaskPool::m_Instance;

bool ITaskPool::Install(BranchTrampoline& a_trampoline)
{
    return Hook::Call5(
        a_trampoline,
        m_hookTargetAddr,
        std::uintptr_t(MainLoopUpdate_Hook),
        m_Instance.mainLoopUpdate_o);
}


void ITaskPool::MainLoopUpdate_Hook(Game::BSMain* a_main)
{
    m_Instance.mainLoopUpdate_o(a_main);
    m_Instance.m_queue.ProcessTasks();
}

static bool IsREFRValid(const TESObjectREFR* a_refr)
{
    if (a_refr == nullptr ||
        a_refr->loadedState == nullptr ||
        (a_refr->flags & TESForm::kFlagIsDeleted) == TESForm::kFlagIsDeleted)
    {
        return false;
    }
    return true;
}


void ITaskPool::QueueActorTask(
    TESObjectREFR* a_actor,
    std::function<void(Actor*)> a_func)
{
    if (!IsREFRValid(a_actor)) {
        return;
    }

    if (a_actor->formType != Actor::kTypeID) {
        return;
    }

    auto handle = a_actor->GetHandle();
    if (!handle.IsValid()) {
        return;
    }

    m_Instance.m_queue.AddTask([handle, func = std::move(a_func)]
        {
            NiPointer<TESObjectREFR> ref;
            if (!handle.LookupREFR(ref)) {
                return;
            }

            if (!IsREFRValid(ref)) {
                return;
            }

            if (ref->formType != Actor::kTypeID) {
                return;
            }

            func(static_cast<Actor*>(ref.get()));
        });
}