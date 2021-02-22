#pragma once

#include <random>

template <class T = float, class = std::enable_if_t<std::is_fundamental_v<T>, void>>
class RandomNumberGenerator
{
public:

    using producer_type =
        std::conditional_t<std::is_floating_point_v<T>, std::uniform_real_distribution<T>,
        std::conditional_t<std::is_integral_v<T>, std::uniform_int_distribution<T>, void>>;

    explicit RandomNumberGenerator(T a_min, T a_max) :
        m_generator(m_device()),
        m_producer(a_min, a_max)
    {
    }

    virtual ~RandomNumberGenerator() noexcept = default;

    virtual T Get()
    {
        return m_producer(m_generator);
    }

protected:
    std::random_device m_device;
    std::mt19937 m_generator;
    producer_type m_producer;
};

#include <common/ICriticalSection.h>

template <class T = float>
class ThreadSafeRandomNumberGenerator :
    public RandomNumberGenerator<T>
{
public:

    using RandomNumberGenerator<T>::RandomNumberGenerator;

    virtual T Get()
    {
        IScopedCriticalSection _(std::addressof(m_lock));
        return m_producer(m_generator);
    }

private:
    ICriticalSection m_lock;
};