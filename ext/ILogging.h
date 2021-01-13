#pragma once

#include "IMisc.h"

#include <unordered_map>
#include <string>
#include <sstream>

#include "skse64/common/IDebugLog.h"

class ILog
{
public:
    template<typename... Args>
    SKMP_FORCEINLINE void Debug(const char* a_fmt, Args... a_args) const
    {
        gLog.Debug(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    SKMP_FORCEINLINE void Message(const char* a_fmt, Args... a_args) const
    {
        gLog.Message(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    SKMP_FORCEINLINE void Warning(const char* a_fmt, Args... a_args) const
    {
        gLog.Warning(FormatString(a_fmt, "WARNING").c_str(), a_args...);
    }

    template<typename... Args>
    SKMP_FORCEINLINE void Error(const char* a_fmt, Args... a_args) const
    {
        gLog.Error(FormatString(a_fmt, "ERROR").c_str(), a_args...);
    }

    template<typename... Args>
    SKMP_FORCEINLINE void FatalError(const char* a_fmt, Args... a_args) const
    {
        gLog.FatalError(FormatString(a_fmt, "FATAL").c_str(), a_args...);
    }

    SKMP_FORCEINLINE void LogPatchBegin(const char* a_id) const
    {
        gLog.Debug(FormatString("[Patch] [%s] Writing..").c_str(), a_id);
    }

    SKMP_FORCEINLINE void LogPatchEnd(const char* a_id) const
    {
        gLog.Debug(FormatString("[Patch] [%s] OK").c_str(), a_id);
    }

    static IDebugLog::LogLevel TranslateLogLevel(const std::string& a_level);

    FN_NAMEPROC("ILog")
private:

    SKMP_FORCEINLINE std::string FormatString(const char* a_fmt, const char* a_pfix = nullptr) const
    {
        std::ostringstream fmt;

        if (a_pfix != nullptr)
            fmt << "<" << a_pfix << "> ";

        fmt << "[" << ModuleName() << "] " << a_fmt;

        return fmt.str();
    }

    typedef std::unordered_map<std::string, IDebugLog::LogLevel> logLevelMap_t;
    static logLevelMap_t m_logLevelMap;

};

class BackLog
{
    typedef std::vector<std::string> vec_t;

    using iterator = typename vec_t::iterator;
    using const_iterator = typename vec_t::const_iterator;

public:

    using size_type = vec_t::size_type;

    BackLog(size_t a_limit) :
        m_limit(a_limit)
    {
    }

    [[nodiscard]] SKMP_FORCEINLINE const_iterator begin() const noexcept {
        return m_data.begin();
    }
    [[nodiscard]] SKMP_FORCEINLINE const_iterator end() const noexcept {
        return m_data.end();
    }

    SKMP_FORCEINLINE void Lock() {
        m_lock.Enter();
    }

    SKMP_FORCEINLINE void Unlock() {
        m_lock.Leave();
    }

    SKMP_FORCEINLINE auto& GetLock() {
        return m_lock;
    }

    SKMP_FORCEINLINE void Add(const char* a_string)
    {
        IScopedCriticalSection _(&m_lock);

        m_data.emplace_back(a_string);
        if (m_data.size() > m_limit)
            m_data.erase(m_data.begin());
    }

    SKMP_FORCEINLINE void SetLimit(size_type a_limit)
    {
        IScopedCriticalSection _(&m_lock);

        m_limit = std::max<size_type>(a_limit, 1);

        while (m_data.size() > m_limit)
            m_data.erase(m_data.begin());
    }

private:
    ICriticalSection m_lock;

    vec_t m_data;
    size_type m_limit;
};