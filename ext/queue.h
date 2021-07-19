#pragma once

#include <queue>
#include <list>
#include <mutex>
#include <condition_variable>

template <typename T>
class ProducerConsumerQueue
{
private:
    using scoped_lock_type = std::unique_lock<std::mutex>;
public:
    T pop()
    {
        scoped_lock_type mlock(m_mutex);
        while (m_queue.empty()) {
            m_cond.wait(mlock);
        }
        auto val = std::move(m_queue.front());
        m_queue.pop();
        return val;
    }

    void pop(T& item)
    {
        scoped_lock_type mlock(m_mutex);
        while (m_queue.empty()) {
            m_cond.wait(mlock);
        }
        item = std::move(m_queue.front());
        m_queue.pop();
    }

    void push(const T& item)
    {
        scoped_lock_type mlock(m_mutex);
        m_queue.emplace(item);
        mlock.unlock();
        m_cond.notify_one();
    }

    void push(T&& item)
    {
        scoped_lock_type mlock(m_mutex);
        m_queue.emplace(std::move(item));
        mlock.unlock();
        m_cond.notify_one();
    }

    template <typename... Args>
    void push(Args&& ...a_args)
    {
        scoped_lock_type mlock(m_mutex);
        m_queue.emplace(T(std::forward<Args>(a_args)...));
        mlock.unlock();
        m_cond.notify_one();
    }

    bool queue_empty()
    {
        scoped_lock_type mlock(m_mutex);
        return m_queue.empty();
    }

    ProducerConsumerQueue() = default;
    ProducerConsumerQueue(const ProducerConsumerQueue&) = delete;
    ProducerConsumerQueue& operator=(const ProducerConsumerQueue&) = delete;

private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};


/*template <typename T>
class IQueueEx
{
private:
    typedef std::unique_lock<std::mutex> UniqueLock;

public:
    T pop()
    {
        UniqueLock mlock(m_mutex);
        while (m_queue.empty())
        {
            m_cond.wait(mlock);
        }
        auto val = m_queue.front();
        m_queue.pop_front();
        return val;
    }

    void pop(T& item)
    {
        UniqueLock mlock(m_mutex);
        while (m_queue.empty())
        {
            m_cond.wait(mlock);
        }
        item = m_queue.front();
        m_queue.pop_front();
    }

    void push(const T& item)
    {
        UniqueLock mlock(m_mutex);
        m_queue.push_back(item);
        mlock.unlock();
        m_cond.notify_one();
    }

    void push_front(const T& item)
    {
        UniqueLock mlock(m_mutex);
        m_queue.push_front(item);
        mlock.unlock();
        m_cond.notify_one();
    }

    bool queue_empty()
    {
        UniqueLock mlock(m_mutex);
        return m_queue.empty();
    }

    void erase_first_if(std::function<bool(T&)> const& fn)
    {
        UniqueLock mlock(m_mutex);
        for (auto it = m_queue.begin(); it != m_queue.end(); ++it) {
            if (fn(*it)) {
                m_queue.erase(it);
                break;
            }
        }
    }

    IQueueEx() = default;
    IQueueEx(const IQueueEx&) = delete;
    IQueueEx& operator=(const IQueueEx&) = delete;
private:
    stl::list_simd<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;
};
*/