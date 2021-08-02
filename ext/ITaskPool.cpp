#include "ITaskPool.h"

#include <ext/JITASM.h>
#include <ext/Patching.h>

#include <skse64/GameReferences.h>
#include <skse64/FormTraits.h>

ITaskPool ITaskPool::m_Instance;

void ITaskPool::Install(
    BranchTrampoline& a_branchTrampoline,
    BranchTrampoline& a_localTrampoline)
{
    struct Assembly : JITASM::JITASM
    {
        Assembly(
            BranchTrampoline& a_localTrampoline,
            std::uintptr_t a_targetAddr, 
            bool a_chain)
            :
            JITASM(a_localTrampoline)
        {
            Xbyak::Label retnLabel;
            Xbyak::Label callLabel;

            call(ptr[rip + callLabel]);

            if (!a_chain) {
                db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), 0x5);
            }

            jmp(ptr[rip + retnLabel]);

            L(retnLabel);
            if (a_chain) {
                dq(a_targetAddr);
            }
            else {
                dq(a_targetAddr + 0x5);
            }

            L(callLabel);
            dq(std::uintptr_t(MainLoopUpdate_Hook));
        }
    };

    auto addr = m_hookTargetAddr;

    std::uintptr_t jmpAddr;
    bool chain = Hook::GetDst5<0xE9>(addr, jmpAddr);
    if (!chain) 
    {
        jmpAddr = addr;
    }

    Assembly code(a_localTrampoline, jmpAddr, chain);
    a_branchTrampoline.Write5Branch(addr, code.get());
}

bool ITaskPool::ValidateMemory()
{
    constexpr std::uint8_t mem1[]{ 0x48, 0x8B, 0x5C, 0x24, 0x40, 0x48, 0x83, 0xC4, 0x30 };
    constexpr std::uint8_t mem2[]{ 0xE9 };

    return 
        Patching::validate_mem(m_hookTargetAddr, mem1) ||
        Patching::validate_mem(m_hookTargetAddr, mem2);
}

void ITaskPool::MainLoopUpdate_Hook()
{
    m_Instance.m_queue.ProcessTasks();

    for (auto const& cmd : m_Instance.m_tasks_fixed) {
        cmd->Run();
    }
}

static bool IsREFRValid(TESObjectREFR* a_refr)
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
    func_t a_func)
{
    if (!IsREFRValid(a_actor)) {
        return;
    }

    auto actor = a_actor->As<Actor>();
    if (!actor) {
        return;
    }

    auto handle = actor->GetHandle();
    if (!handle.IsValid()) {
        return;
    }

    m_Instance.m_queue.AddTask([handle, func = std::move(a_func)]()
    {
        NiPointer<Actor> actor;
        if (!handle.Lookup(actor)) {
            return;
        }

        if (!IsREFRValid(actor)) {
            return;
        }

        func(actor, handle);
    });
}
