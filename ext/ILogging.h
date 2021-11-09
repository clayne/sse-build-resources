#pragma once

#include "Threads.h"
#include "STL.h"

class ILog
{
    inline static constexpr std::size_t FORMAT_BUFFER_SIZE = 8192;

public:

    template<typename... Args>
    void Debug(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(LogLevel::Debug))
            gLog.Write(LogLevel::Debug, a_fmt, LogPrefix(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void Message(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(LogLevel::Message))
            gLog.Write(LogLevel::Message, a_fmt, LogPrefix(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void VMessage(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(LogLevel::Verbose))
            gLog.Write(LogLevel::Verbose, a_fmt, LogPrefix(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void Warning(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(LogLevel::Warning))
            gLog.Write(LogLevel::Warning, a_fmt, LogPrefixWarning(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void Error(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(LogLevel::Error))
            gLog.Write(LogLevel::Error, a_fmt, LogPrefixError(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void FatalError(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(LogLevel::FatalError))
            gLog.Write(LogLevel::FatalError, a_fmt, LogPrefixFatal(), std::forward<Args>(a_args)...);
    }

    SKMP_FORCEINLINE void LogPatchBegin(const char* a_id) const
    {
        Debug("[Patch] [%s] Writing..", a_id);
    }

    SKMP_FORCEINLINE void LogPatchEnd(const char* a_id) const
    {
        Debug("[Patch] [%s] OK", a_id);
    }

    using logLevelMap_t = stl::iunordered_map<std::string, LogLevel>;

    static const logLevelMap_t m_logLevelMap;

    static LogLevel TranslateLogLevel(const std::string& a_level);
    static const char* GetLogLevelString(LogLevel a_level);

    inline static constexpr const auto& GetLogLevels() noexcept
    {
        return m_logLevelMap;
    }

    FN_NAMEPROC("ILog")
private:

};

class BackLog
{
    class Entry
    {
    public:
        using size_type = std::uint32_t;

        Entry() = delete;

        explicit Entry(const LoggerMessageEvent& a_event);

        Entry(const Entry&) = delete;
        explicit Entry(Entry&&) = default;

        Entry& operator=(const Entry&) = delete;
        Entry& operator=(Entry&&) = default;

        inline constexpr const char* data() const noexcept
        {
            return m_data.data();
        }

        inline constexpr operator const auto& () const noexcept
        {
            return m_data;
        }

        inline constexpr auto level() const noexcept
        {
            return m_level;
        }

    private:

        std::string m_data;
        LogLevel m_level;
    };

    using storage_type = std::list<Entry>;
    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;

public:

    using size_type = storage_type::size_type;

    inline BackLog(std::size_t a_limit = 0) :
        m_limit(a_limit)
    {
    }

    [[nodiscard]] inline const_iterator begin() const noexcept
    {
        return m_data.begin();
    }

    [[nodiscard]] inline const_iterator end() const noexcept
    {
        return m_data.end();
    }

    inline void Lock() const noexcept
    {
        m_lock.lock();
    }

    inline void Unlock() const noexcept
    {
        m_lock.unlock();
    }

    [[nodiscard]] inline auto& GetLock() const noexcept
    {
        return m_lock;
    }

    [[nodiscard]] inline auto GetLimit() const noexcept
    {
        return m_limit;
    }

    [[nodiscard]] inline auto Size() const noexcept
    {
        return m_data.size();
    }

    void Add(const LoggerMessageEvent& a_event);
    void SetLimit(size_type a_limit);
    void Clear();

private:
    mutable WCriticalSection m_lock;

    storage_type m_data;
    size_type m_limit;
};
