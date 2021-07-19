#pragma once

#include "workerpool.h"

class TaskObject
{
public:

    virtual ~TaskObject() noexcept = default;

    virtual void Run() = 0;
};

class TaskObjectFunctor :
    public TaskObject
{
public:
    using func_t = std::function<void()>;

    TaskObjectFunctor(func_t&& a_func)
        : m_func(std::move(a_func))
    {
    }

    TaskObjectFunctor(const func_t& a_func)
        : m_func(a_func)
    {
    }

    virtual ~TaskObjectFunctor() noexcept = default;

    virtual void Run()
    {
        m_func();
    };

private:

    func_t m_func;
};

struct TaskObjectWrapper
{
private:

    template <class T>
    using is_valid_object = std::enable_if_t<
        std::is_base_of_v<TaskObject, T>, int>;


public:
    TaskObjectWrapper() = default;

    template <class T, is_valid_object<T> = 0>
    TaskObjectWrapper(T* a_task) :
        m_task(a_task)
    {
    }
    
    template <class T, is_valid_object<T> = 0>
    TaskObjectWrapper(std::unique_ptr<T>&& a_task) :
        m_task(std::move(a_task))
    {
    }

    std::unique_ptr<TaskObject> m_task;
};

class ITaskWorkerPool :
    public IWorkerPool<TaskObjectWrapper>
{
public:

    using task_func_t = std::function<void()>;
    using task_list_t = std::vector<task_func_t>;

    using IWorkerPool<TaskObjectWrapper>::IWorkerPool;

    template <class T, typename... Args>
    void Create(Args&& ...a_args);

    void Create(TaskObjectFunctor::func_t a_func);

    /*void RunConcurrent(std::size_t a_num, const std::function<void(std::size_t)> &a_func);
    void RunConcurrent(const std::function<void(std::size_t)>& a_func);*/

    void RunConcurrentTasks(const task_list_t& a_tasks);

protected:
    virtual void Process(const TaskObjectWrapper& a_task);
};

template <class T, typename... Args>
void ITaskWorkerPool::Create(Args&& ...a_args)
{
    static_assert(std::is_base_of<TaskObject, T>::value);

    Push(new T(std::forward<Args>(a_args)...));
}
