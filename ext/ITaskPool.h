#pragma once

#include "AddressLibrary.h"
#include "ITasks.h"

#include "Threads.h"

namespace Game
{
	class BSMain;
}

class ITaskPool
{
	using func_t = std::function<void(Actor*, Game::ActorHandle)>;

public:
	ITaskPool() = default;

	static void Install(
		BranchTrampoline& a_branchTrampoline,
		BranchTrampoline& a_localTrampoline);

	static bool ValidateMemory();

	static void ITaskPool::QueueActorTask(
		TESObjectREFR* a_actor,
		func_t a_func);

	inline static void AddTask(TaskFunctor::func_t a_func)
	{
		m_Instance.m_queue.AddTask(std::move(a_func));
	}

	inline static void AddTask(TaskDelegate* cmd)
	{
		m_Instance.m_queue.AddTask(cmd);
	}

	template <class T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<TaskDelegate, T>>>
	static void AddTask(Args&&... a_args)
	{
		m_Instance.m_queue.AddTask<T>(std::forward<Args>(a_args)...);
	}

	inline static void AddTaskFixed(TaskDelegateFixed* cmd)
	{
		m_Instance.m_tasks_fixed.emplace_back(cmd);
	}

	inline static void SetBudget(long long a_budget)
	{
		m_Instance.m_budget = a_budget;
	}

private:
	static void MainLoopUpdate_Hook();
	static void MainLoopUpdate_Hook_Budget();

	TaskQueue m_queue;

	typedef void (*mainLoopUpdate_t)(Game::BSMain*);
	mainLoopUpdate_t mainLoopUpdate_o;

	std::vector<TaskDelegateFixed*> m_tasks_fixed;
	long long m_budget{ 0 };

	inline static auto m_hookTargetAddr = IAL::Addr(35565, 36564, 0x759, 0xC37);

	static ITaskPool m_Instance;
};