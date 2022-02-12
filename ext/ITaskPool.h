#pragma once

#include "AddressLibrary.h"
#include "ITasks.h"

#include "Threads.h"

namespace Game
{
	class Main;
}

class ITaskPool
{
	using func_t = std::function<void(Actor*, Game::ActorHandle)>;

	class ActorTaskDispatcher :
		public TaskDelegate
	{
	public:
		ActorTaskDispatcher(
			Game::ActorHandle a_handle,
			func_t&& a_func);

		virtual void Run();

		virtual void Dispose()
		{
			delete this;
		}

	private:
		Game::ActorHandle m_handle;
		func_t m_func;
	};

public:
	ITaskPool() = default;

	static void Install(
		BranchTrampoline& a_branchTrampoline,
		BranchTrampoline& a_localTrampoline);

	static bool ValidateMemory();

	static void ITaskPool::QueueActorTask(
		TESObjectREFR* a_actor,
		func_t a_func);

	static void ITaskPool::QueueActorTask(
		Game::ActorHandle a_handle,
		func_t a_func);

	inline static void AddTask(TaskFunctor::func_t a_func)
	{
		m_Instance.m_queue.AddTask(std::move(a_func));
	}

	inline static void AddTask(TaskDelegate* cmd)
	{
		m_Instance.m_queue.AddTask(cmd);
	}

	template <class T, class... Args, class = std::enable_if_t<std::is_base_of_v<TaskDelegate, T>>>
	static constexpr void AddTask(Args&&... a_args)
	{
		m_Instance.m_queue.AddTask<T>(std::forward<Args>(a_args)...);
	}

	inline static void AddPriorityTask(TaskFunctor::func_t a_func)
	{
		m_Instance.m_prioQueue.AddTask(std::move(a_func));
	}

	inline static void AddPriorityTask(TaskDelegate* cmd)
	{
		m_Instance.m_prioQueue.AddTask(cmd);
	}

	template <class T, class... Args, class = std::enable_if_t<std::is_base_of_v<TaskDelegate, T>>>
	static constexpr void AddPriorityTask(Args&&... a_args)
	{
		m_Instance.m_prioQueue.AddTask<T>(std::forward<Args>(a_args)...);
	}

	inline static constexpr void AddTaskFixed(TaskDelegateFixed* cmd)
	{
		m_Instance.m_tasks_fixed.emplace_back(cmd);
	}

	inline static constexpr void SetBudget(long long a_budget) noexcept
	{
		m_Instance.m_budget = a_budget;
	}

	inline static bool IsRunningOnCurrentThread() noexcept
	{
		return GetCurrentThreadId() ==
		       m_Instance.m_runningThread.load(std::memory_order_relaxed);
	}

private:
	static void MainLoopUpdate_Hook();
	static void MainLoopUpdate_Hook_Budget();

	TaskQueue m_queue;
	TaskQueue m_prioQueue;

	std::vector<TaskDelegateFixed*> m_tasks_fixed;
	long long m_budget{ 0 };

	std::atomic<DWORD> m_runningThread{ 0 };

	inline static const auto m_hookTargetAddr = IAL::Addr(35565, 36564, 0x759, 0xC37);

	static ITaskPool m_Instance;
};