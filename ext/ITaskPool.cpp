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
	auto addr = m_hookTargetAddr;
	std::uintptr_t jmpAddr;

    if (IAL::IsAE())
    {
		struct Assembly : JITASM::JITASM
		{
			Assembly(
				BranchTrampoline& a_localTrampoline,
				std::uintptr_t a_targetAddr,
				bool a_chain,
				bool a_budget) :
				JITASM(a_localTrampoline)
			{
				Xbyak::Label retnLabel;
				Xbyak::Label callLabel;

				call(ptr[rip + callLabel]);

				if (!a_chain)
				{
					db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), 0x8);
				}

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				if (a_chain)
				{
					dq(a_targetAddr);
				}
				else
				{
					dq(a_targetAddr + 0x8);
				}

				L(callLabel);
				dq(std::uintptr_t(a_budget ? MainLoopUpdate_Hook_Budget : MainLoopUpdate_Hook));
			}
		};

		bool chain = Hook::GetDst6<0x25>(addr, jmpAddr);
		if (!chain)
		{
			jmpAddr = addr;
		}

		Assembly code(a_localTrampoline, jmpAddr, chain, m_Instance.m_budget > 0);
		a_branchTrampoline.Write6Branch(addr, code.get());
    }
	else
	{
		struct Assembly : JITASM::JITASM
		{
			Assembly(
				BranchTrampoline& a_localTrampoline,
				std::uintptr_t a_targetAddr,
				bool a_chain,
				bool a_budget) :
				JITASM(a_localTrampoline)
			{
				Xbyak::Label retnLabel;
				Xbyak::Label callLabel;

				call(ptr[rip + callLabel]);

				if (!a_chain)
				{
					db(reinterpret_cast<Xbyak::uint8*>(a_targetAddr), 0x5);
				}

				jmp(ptr[rip + retnLabel]);

				L(retnLabel);
				if (a_chain)
				{
					dq(a_targetAddr);
				}
				else
				{
					dq(a_targetAddr + 0x5);
				}

				L(callLabel);
				dq(std::uintptr_t(a_budget ? MainLoopUpdate_Hook_Budget : MainLoopUpdate_Hook));
			}
		};

		bool chain = Hook::GetDst5<0xE9>(addr, jmpAddr);
		if (!chain)
		{
			jmpAddr = addr;
		}

		Assembly code(a_localTrampoline, jmpAddr, chain, m_Instance.m_budget > 0);
		a_branchTrampoline.Write5Branch(addr, code.get());
	}

    

}

bool ITaskPool::ValidateMemory()
{
    if (IAL::IsAE())
	{
		constexpr std::uint8_t mem2[]{ 0xFF, 0x25 };
		constexpr std::uint8_t mem1[]{ 0x4C, 0x8D, 0x9C, 0x24, 0x30, 0x01, 0x00, 0x00 };
		return Patching::validate_mem(m_hookTargetAddr, mem1) ||
		       Patching::validate_mem(m_hookTargetAddr, mem2);
	}
    else
	{
		constexpr std::uint8_t mem2[]{ 0xE9 };
		constexpr std::uint8_t mem1[]{ 0x48, 0x8B, 0x5C, 0x24, 0x40, 0x48, 0x83, 0xC4, 0x30 };
		return Patching::validate_mem(m_hookTargetAddr, mem1) ||
		       Patching::validate_mem(m_hookTargetAddr, mem2);
    }
}

void ITaskPool::MainLoopUpdate_Hook()
{
    m_Instance.m_queue.ProcessTasks();
    //m_Instance.m_queueSecondary.ProcessTasks();

    for (auto const& cmd : m_Instance.m_tasks_fixed)
    {
        cmd->Run();
    }
}

void ITaskPool::MainLoopUpdate_Hook_Budget()
{
    m_Instance.m_queue.ProcessTasks(m_Instance.m_budget);
    //m_Instance.m_queueSecondary.ProcessTasks(m_Instance.m_budget);

    for (auto const& cmd : m_Instance.m_tasks_fixed)
    {
        cmd->Run();
    }
}

static bool IsREFRValid(TESObjectREFR* a_refr)
{
    if (a_refr == nullptr ||
        a_refr->formID == 0 ||
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
    if (!IsREFRValid(a_actor))
    {
        return;
    }

    auto actor = a_actor->As<Actor>();
    if (!actor)
    {
        return;
    }

    auto handle = actor->GetHandle();
    if (!handle || !handle.IsValid())
    {
        return;
    }

    m_Instance.m_queue.AddTask([handle, func = std::move(a_func)]()
    {
        NiPointer<Actor> actor;
        if (!handle.Lookup(actor))
        {
            return;
        }

        if (!IsREFRValid(actor))
        {
            return;
        }

        func(actor, handle);
    });
}

/*void ITaskPool::QueueActorTaskSecondary(
    TESObjectREFR* a_actor,
    func_t a_func)
{
    if (!IsREFRValid(a_actor))
    {
        return;
    }

    auto actor = a_actor->As<Actor>();
    if (!actor)
    {
        return;
    }

    auto handle = actor->GetHandle();
    if (!handle || !handle.IsValid())
    {
        return;
    }

    m_Instance.m_queueSecondary.AddTask([handle, func = std::move(a_func)]()
    {
        NiPointer<Actor> actor;
        if (!handle.Lookup(actor))
        {
            return;
        }

        if (!IsREFRValid(actor))
        {
            return;
        }

        func(actor, handle);
    });
}*/
