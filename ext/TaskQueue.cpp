
#include "TaskQueue.h"

#include <mutex>

void ITaskWorkerPool::Process(const TaskObjectWrapper& a_task)
{
    a_task.m_task->Run();
}

void ITaskWorkerPool::Create(TaskObjectFunctor::func_t a_func)
{
    Push(std::make_unique<TaskObjectFunctor>(std::move(a_func)));
}

/*void IWorkerPoolTaskQueue::RunConcurrent(std::size_t a_num, const std::function<void(std::size_t)>& a_func)
{
    if (a_num == 0ULL)
        throw std::invalid_argument("a_num == 0");

    volatile std::size_t c = 0ULL;

    std::mutex mutex;
    std::condition_variable cond;

    std::unique_lock<std::mutex> lock(mutex);

    for (std::size_t i = 0; i < a_num; i++)
    {
        Create([&, i]()
        {
            try
            {
                a_func(i);
            }
            catch (...)
            {
            }

            if (InterlockedIncrementSizeT(&c) == a_num)
                cond.notify_one();
        });
    }

    cond.wait(lock, [&] { return c == a_num; });
}

void IWorkerPoolTaskQueue::RunConcurrent(const std::function<void(std::size_t)>& a_func)
{
    return RunConcurrent(Size(), a_func);
}*/

void ITaskWorkerPool::RunConcurrentTasks(const task_list_t& a_tasks)
{
    std::size_t num = a_tasks.size();

    if (!num)
        return;

    volatile std::size_t c = 0ULL;

    std::mutex mutex;
    std::condition_variable cond;
    std::unique_lock<std::mutex> lock(mutex);

    for (auto &func : a_tasks)
    {
        Create([&, num]()
            {
                try
                {
                    func();
                }
                catch (...)
                {
                }

                if (InterlockedIncrementAcquire(&c) == num)
                    cond.notify_one();
            });
    }

    cond.wait(lock, [&] { return c == num; });
}