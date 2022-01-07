#pragma once

#include "STLCommon.h"

#include <vector>

namespace Events
{
    template <class T>
    class EventSink
    {
        using event_type = stl::strip_type<T>;

    public:
        virtual void Receive(const event_type& a_evn) = 0;
    };

    template <class T>
    class EventDispatcher
    {
        using event_type = stl::strip_type<T>;
        using sink_type = EventSink<event_type>;
        using storage_type = std::vector<sink_type*>;

    public:
        virtual ~EventDispatcher() noexcept = default;

        void SendEvent(const event_type& a_evn) const
        {
            for (auto& e : m_sinks)
            {
                e->Receive(a_evn);
            }
        }

        constexpr void AddSink(sink_type* a_sink)
        {
            if (find(a_sink) == m_sinks.end())
            {
                m_sinks.emplace_back(a_sink);
            }
        }

        constexpr void RemoveSink(sink_type* a_sink) noexcept
        {
            auto it = find(a_sink);
            if (it != m_sinks.end())
            {
                m_sinks.erase(it);
            }
        }

        inline constexpr auto Size() const noexcept
        {
            return m_sinks.size();
        }

        inline constexpr bool Empty() const noexcept
        {
            return m_sinks.empty();
        }

    private:
        constexpr decltype(auto) find(sink_type* a_sink) const noexcept
        {
            return std::find(m_sinks.cbegin(), m_sinks.cend(), a_sink);
        }

        constexpr decltype(auto) find(sink_type* a_sink) noexcept
        {
            return std::find(m_sinks.begin(), m_sinks.end(), a_sink);
        }

        storage_type m_sinks;
    };
}

