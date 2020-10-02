#pragma once

#include "IMisc.h"

#include <unordered_map>
#include <string>
#include <sstream>

class ILog
{
public:
    template<typename... Args>
    inline void Debug(const char* a_fmt, Args... a_args) const
    {
        gLog.Debug(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    inline void Message(const char* a_fmt, Args... a_args) const
    {
        gLog.Message(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    inline void Warning(const char* a_fmt, Args... a_args) const
    {
        gLog.Warning(FormatString(a_fmt, "WARNING").c_str(), a_args...);
    }

    template<typename... Args>
    inline void Error(const char* a_fmt, Args... a_args) const
    {
        gLog.Error(FormatString(a_fmt, "ERROR").c_str(), a_args...);
    }

    template<typename... Args>
    inline void FatalError(const char* a_fmt, Args... a_args) const
    {
        gLog.FatalError(FormatString(a_fmt, "FATAL").c_str(), a_args...);
    }

    inline void LogPatchBegin(const char* a_id) const
    {
        gLog.Debug(FormatString("[Patch] [%s] Writing..").c_str(), a_id);
    }

    inline void LogPatchEnd(const char* a_id) const
    {
        gLog.Debug(FormatString("[Patch] [%s] OK").c_str(), a_id);
    }

    static IDebugLog::LogLevel TranslateLogLevel(const std::string& a_level);

    FN_NAMEPROC("ILog")
private:

    inline std::string FormatString(const char* a_fmt, const char* a_pfix = nullptr) const
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

    BackLog(size_t a_limit) :
        m_limit(a_limit)
    {
    }

    [[nodiscard]] inline const_iterator begin() const noexcept {
        return m_data.begin();
    }
    [[nodiscard]] inline const_iterator end() const noexcept {
        return m_data.end();
    }

    inline void Lock() {
        m_lock.Enter();
    }

    inline void Unlock() {
        m_lock.Leave();
    }

    inline auto& GetLock() {
        return m_lock;
    }

    inline void Add(const char* a_string)
    {
        m_lock.Enter();

        m_data.emplace_back(a_string);
        if (m_data.size() > m_limit)
            m_data.erase(m_data.begin());

        m_lock.Leave();
    }
private:
    ICriticalSection m_lock;
    std::vector<std::string> m_data;

    size_t m_limit;
};
