#pragma once

#include "ITasks.h"
#include "AddressLibrary.h"

#include "Threads.h"

namespace Game
{
    class BSMain;
}

class ITaskPool
{
public:

    ITaskPool() = default;

    static void Install(
        BranchTrampoline& a_branchTrampoline,
        BranchTrampoline& a_localTrampoline);

    static bool ValidateMemory();

    static void ITaskPool::QueueActorTask(
        TESObjectREFR* a_actor,
        std::function<void(Actor*, Game::ObjectRefHandle)> a_func);
    
    SKMP_FORCEINLINE static void AddTask(TaskFunctor::func_t a_func)
    {
        m_Instance.m_queue.AddTask(std::move(a_func));
    }

    SKMP_FORCEINLINE static void AddTask(TaskDelegate* cmd) {
        m_Instance.m_queue.AddTask(cmd);
    }

    template <class T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<TaskDelegate, T>>>
    static void AddTask(Args&&... a_args)
    {
        m_Instance.m_queue.AddTask<T>(std::forward<Args>(a_args)...);
    }

private:

    static void ITaskPool::MainLoopUpdate_Hook();

    TaskQueue m_queue;

    typedef void(*mainLoopUpdate_t)(Game::BSMain*);
    mainLoopUpdate_t mainLoopUpdate_o;

    inline static auto m_hookTargetAddr = IAL::Addr(35565, 0x759);

    static ITaskPool m_Instance;
};