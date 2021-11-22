#pragma once

#include <queue>
#include <functional>

#include <skse64/gamethreads.h>

#include "Threads.h"
#include "STL.h"

class TaskFunctor :
    public TaskDelegate
{
public:

    using func_t = std::function<void()>;

    TaskFunctor(func_t&& a_func) :
        m_func(std::move(a_func))
    {
    }

    TaskFunctor(const func_t& a_func) :
        m_func(a_func)
    {
    }

    virtual void Run()
    {
        m_func();
    }

    virtual void Dispose()
    {
        delete this;
    }

private:
    func_t m_func;
};

class TaskDelegateStatic
    : public TaskDelegate
{
public:
    virtual void Dispose() override
    {
    };
};

class TaskDelegateFixed
{
public:
    virtual void Run() = 0;
};

template <class N>
class TaskQueueBase
{
    using element_type = std::remove_reference_t<std::remove_cv_t<N>>;
    using base_type = std::remove_pointer_t<element_type>;

    template <class T>
    using strip_type = std::remove_pointer_t<std::remove_reference_t<std::remove_cv_t<T>>>;

    template <class T>
    using is_base_type = std::enable_if_t<std::is_base_of_v<base_type, strip_type<T>>>;

    template <class T>
    using is_pointer = std::enable_if_t<std::is_pointer_v<T>>;

    template <class T>
    using is_not_pointer = std::enable_if_t<!std::is_pointer_v<T>>;

public:

    template <class T, class... Args, class = is_pointer<element_type>, class = is_base_type<T>>
    void AddTask(Args&&... a_args)
    {
        using alloc_type = typename strip_type<T>;

        IScopedLock _(m_lock);
        m_queue.emplace(new alloc_type(std::forward<Args>(a_args)...));
    }

    template <class... Args, class = is_not_pointer<element_type>>
    void AddTask(Args&&... a_args)
    {
        IScopedLock _(m_lock);
        m_queue.emplace(std::forward<Args>(a_args)...);
    }

    template <class = is_pointer<element_type>, class = is_base_type<TaskFunctor>>
    void AddTask(TaskFunctor::func_t a_func)
    {
        IScopedLock _(m_lock);
        m_queue.emplace(new TaskFunctor(std::move(a_func)));
    }

    template <class = is_pointer<element_type>>
    void AddTask(element_type a_item)
    {
        IScopedLock _(m_lock);
        m_queue.emplace(a_item);
    }

    SKMP_FORCEINLINE auto Size() const noexcept
    {
        return m_queue.size();
    }

protected:
    TaskQueueBase() = default;

    SKMP_FORCEINLINE bool TaskQueueEmpty() const
    {
        IScopedLock _(m_lock);
        return m_queue.empty();
    }

    std::queue<element_type> m_queue;
    mutable FastSpinLock m_lock;
};

class TaskQueue :
    public TaskQueueBase<TaskDelegate*>
{
public:

    void ProcessTasks();
    void ProcessTasks(long long a_budget);
    void ClearTasks();
};

template <class T>
class TaskQueueStatic :
    public TaskQueueBase<T>
{
public:

    void ProcessTasks();
    void ClearTasks();
};

template<class T>
void TaskQueueStatic<T>::ProcessTasks()
{
    for (;;)
    {
        m_lock.lock();
        if (m_queue.empty())
        {
            m_lock.unlock();
            break;
        }

        auto task = m_queue.front();
        m_queue.pop();

        m_lock.unlock();

        task.Run();
    }
}

template<class T>
void TaskQueueStatic<T>::ClearTasks()
{
    m_queue.swap(decltype(m_queue)());
}
