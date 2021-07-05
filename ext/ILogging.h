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
        if (gLog.CheckLogLevel(IDebugLog::LogLevel::Debug))
            gLog.Write(a_fmt, LogPrefix(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void Message(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(IDebugLog::LogLevel::Message))
            gLog.Write(a_fmt, LogPrefix(), std::forward<Args>(a_args)...);
    }
    
    template<typename... Args>
    void VMessage(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(IDebugLog::LogLevel::Verbose))
            gLog.Write(a_fmt, LogPrefix(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void Warning(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(IDebugLog::LogLevel::Warning))
            gLog.Write(a_fmt, LogPrefixWarning(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void Error(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(IDebugLog::LogLevel::Error))
            gLog.Write(a_fmt, LogPrefixError(), std::forward<Args>(a_args)...);
    }

    template<typename... Args>
    void FatalError(const char* a_fmt, Args... a_args) const
    {
        if (gLog.CheckLogLevel(IDebugLog::LogLevel::FatalError))
            gLog.Write(a_fmt, LogPrefixFatal(), std::forward<Args>(a_args)...);
    }

    SKMP_FORCEINLINE void LogPatchBegin(const char* a_id) const
    {
        Debug("[Patch] [%s] Writing..", a_id);
    }

    SKMP_FORCEINLINE void LogPatchEnd(const char* a_id) const
    {
        Debug("[Patch] [%s] OK", a_id);
    }

    typedef stl::iunordered_map<std::string, IDebugLog::LogLevel, std::allocator<std::pair<const std::string, IDebugLog::LogLevel>>> logLevelMap_t;
    static logLevelMap_t m_logLevelMap;

    static IDebugLog::LogLevel TranslateLogLevel(const std::string& a_level);

    FN_NAMEPROC("ILog")
private:

};


class BackLog
{
    class LogString
    {
    public:
        using size_type = std::size_t;

        LogString() = delete;
        ~LogString() noexcept
        {
            if (m_data) {
                _mm_free(m_data);
            }
        }

        explicit LogString(const LoggerMessageEvent& a_event) noexcept;

        LogString(const LogString&) = delete;

        explicit LogString(LogString&& a_rhs) noexcept :
            m_data(nullptr)
        {
            __move(std::move(a_rhs));
        };

        LogString& operator=(const LogString&) = delete;

        LogString& operator=(LogString&& a_rhs) noexcept
        {
            this->~LogString();
            __move(std::move(a_rhs));
            return *this;
        };

        SKMP_FORCEINLINE const char* c_str() const noexcept {
            return m_data;
        }

        SKMP_FORCEINLINE const char* data() const noexcept {
            return m_data;
        }

    private:

        SKMP_FORCEINLINE void __move(LogString&& a_rhs)
        {
            m_data = a_rhs.m_data;
            a_rhs.m_data = nullptr;
        }

        char* m_data;
    };

    using storage_type = std::vector<LogString>;
    using iterator = typename storage_type::iterator;
    using const_iterator = typename storage_type::const_iterator;

public:

    using size_type = storage_type::size_type;

    SKMP_FORCEINLINE BackLog(std::size_t a_limit) :
        m_limit(a_limit)
    {
        m_data.reserve(std::min<size_type>(a_limit, 1000));
    }

    [[nodiscard]] SKMP_FORCEINLINE const_iterator begin() const noexcept {
        return m_data.begin();
    }

    [[nodiscard]] SKMP_FORCEINLINE const_iterator end() const noexcept {
        return m_data.end();
    }

    SKMP_FORCEINLINE void Lock() const noexcept {
        m_lock.lock();
    }

    SKMP_FORCEINLINE void Unlock() const noexcept {
        m_lock.unlock();
    }

    [[nodiscard]] SKMP_FORCEINLINE auto& GetLock() const noexcept {
        return m_lock;
    }

    void Add(const LoggerMessageEvent& a_event);
    void SetLimit(size_type a_limit);

private:
    mutable WCriticalSection m_lock;

    storage_type m_data;
    size_type m_limit;
};
