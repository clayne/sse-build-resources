#include "ITasks.h"
#include "PerfCounter.h"

void TaskQueue::ProcessTasks()
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

        task->Run();
        task->Dispose();
    }
}

void TaskQueue::ProcessTasks(long long a_budget)
{
    auto begin = IPerfCounter::Query();

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

        task->Run();
        task->Dispose();

        if (IPerfCounter::delta_us(begin, IPerfCounter::Query()) > a_budget)
        {
            break;
        }
    }
}

void TaskQueue::ClearTasks()
{
    IScopedLock _(m_lock);

    while (!m_queue.empty())
    {
        auto task = m_queue.front();
        m_queue.pop();

        task->Dispose();
    }
}