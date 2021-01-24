#pragma once

#include <queue>

#include <common/ICriticalSection.h>
#include <skse64/gamethreads.h>

template <class N>
class TaskQueueBase
{
    using element_type = typename std::remove_reference_t<std::remove_cv_t<N>>;

public:

    template <class T, typename... Args, typename = std::enable_if_t<std::is_pointer_v<element_type>>>
    SKMP_FORCEINLINE void AddTask(Args&&... a_args)
    {
        using alloc_type = typename std::remove_pointer_t<std::remove_reference_t<T>>;

        static_assert(
            std::is_base_of_v<std::remove_pointer_t<element_type>, alloc_type>);

        IScopedCriticalSection _(&m_lock);
        m_queue.emplace(new alloc_type(std::forward<Args>(a_args)...));
    }

    template <typename... Args, typename = std::enable_if_t<!std::is_pointer_v<element_type>>>
    SKMP_FORCEINLINE void AddTask(Args&&... a_args)
    {
        IScopedCriticalSection _(&m_lock);
        m_queue.emplace(element_type{ std::forward<Args>(a_args)... });
    }

    template <typename = std::enable_if_t<std::is_pointer_v<element_type>>>
    SKMP_FORCEINLINE void AddTask(element_type a_item)
    {
        IScopedCriticalSection _(&m_lock);
        m_queue.emplace(a_item);
    }

protected:
    TaskQueueBase() = default;

    SKMP_FORCEINLINE bool TaskQueueEmpty()
    {
        IScopedCriticalSection _(&m_lock);
        return m_queue.empty();
    }

    std::queue<element_type> m_queue;
    ICriticalSection m_lock;

private:

};

class TaskQueue :
    public TaskQueueBase<TaskDelegate*>
{
public:

    SKMP_FORCEINLINE void ProcessTasks()
    {
        while (!TaskQueueEmpty())
        {
            m_lock.Enter();

            auto task = m_queue.front();
            m_queue.pop();

            m_lock.Leave();

            task->Run();
            task->Dispose();
        }
    }

    SKMP_FORCEINLINE void ClearTasks()
    {
        IScopedCriticalSection _(&m_lock);

        while (!m_queue.empty())
        {
            auto task = m_queue.front();
            m_queue.pop();

            task->Dispose();
        }

    }
};

class TaskDelegateFixed
{
public:
    virtual void Run() = 0;
};

class TaskDelegateStatic
    : public TaskDelegate
{
public:
    virtual void Run() = 0;
    virtual void Dispose() {};
};