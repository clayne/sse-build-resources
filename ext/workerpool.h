#pragma once

#include "queue.h"

#include <thread>

template <class T>
class IWorkerPool
{
    struct SKMP_ALIGN_AUTO TaskWrapper
    {
        TaskWrapper() : m_stop(true) {}
        TaskWrapper(const T& a_task) : m_task(a_task), m_stop(false) {}
        TaskWrapper(T&& a_task) : m_task(std::move(a_task)), m_stop(false) {}

        bool m_stop;
        T m_task;
    };

    using thread_storage_t = std::vector<std::thread>;

public:

    IWorkerPool();
    IWorkerPool(std::uint32_t a_numThreads);

    void Start();
    void Stop();

    SKMP_FORCEINLINE void Push(T&& a_task);
    SKMP_FORCEINLINE void Push(const T& a_task);
    SKMP_FORCEINLINE thread_storage_t::size_type Size() const;
    SKMP_FORCEINLINE void SetThreadCount(std::uint32_t a_numThreads);

protected:

    virtual ~IWorkerPool() noexcept;

    virtual void Process(const T& a_task) = 0;

private:

    void Pump();

    std::uint32_t m_numThreads;

    ProducerConsumerQueue<TaskWrapper> m_queue;
    thread_storage_t m_threads;
};

template <class T>
IWorkerPool<T>::IWorkerPool()
    :
    m_numThreads(0)
{
}

template <class T>
IWorkerPool<T>::IWorkerPool(
    std::uint32_t a_numThreads)
    :
    m_numThreads(a_numThreads)
{
}

template <class T>
IWorkerPool<T>::~IWorkerPool() noexcept
{
    ASSERT(m_threads.empty());
    //Stop();
}

template <class T>
void IWorkerPool<T>::Start()
{
    if (m_numThreads == 0) {
        throw std::exception("thread count == 0");
    }

    for (std::uint32_t i = 0; i < m_numThreads; i++)
    {
        m_threads.emplace_back(std::bind(&IWorkerPool::Pump, this));
    }
}

template <class T>
void IWorkerPool<T>::Stop()
{
    for (auto& e : m_threads) {
        m_queue.push();
    }

    for (auto& e : m_threads) {
        e.join();
    }

    m_threads.clear();
}


template <class T>
void IWorkerPool<T>::Push(T&& a_task)
{
    m_queue.push(std::move(a_task));
}

template <class T>
void IWorkerPool<T>::Push(const T& a_task)
{
    m_queue.push(a_task);
}

template <class T>
auto IWorkerPool<T>::Size() const
-> thread_storage_t::size_type 
{
    return m_threads.size();
}

template <class T>
void IWorkerPool<T>::SetThreadCount(std::uint32_t a_numThreads)
{
    m_numThreads = a_numThreads;
}

template <class T>
void IWorkerPool<T>::Pump()
{
    for (;;)
    {
        TaskWrapper task{};
        m_queue.pop(task);

        if (task.m_stop)
            break;

        Process(task.m_task);
    }
}