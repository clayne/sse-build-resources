#pragma once

#include "IMisc.h"

#include <memory>
#include <unordered_map>
#include <string>
#include <sstream>
#include <fstream>
#include <common/ICriticalSection.h>

class Logger
{
    static constexpr size_t DEFAULT_BUFFER_SIZE = 8192;

    typedef void (*onWriteCallback_t)(char* a_buffer);
public:
    enum class LogLevel : int {
        FatalError = 0,
        Error,
        Warning,
        Message,
        Debug
    };

    Logger(size_t a_bufferSize = DEFAULT_BUFFER_SIZE);
    Logger(LogLevel a_logLevel, size_t a_bufferSize = DEFAULT_BUFFER_SIZE);
    virtual ~Logger() noexcept;

    bool Open(const char* a_fname);
    bool Open(const char* a_basepath, const char* a_fname);
    bool OpenRelative(int a_fid, const char* a_fname);
    void Close();

    inline void SetLogLevel(LogLevel a_logLevel) {
        m_logLevel = a_logLevel;
    }

    template<typename... Args>
    inline void Debug(const char* a_fmt, Args... a_args)
    {
        if (CheckLogLevel(LogLevel::Debug))
            Write(a_fmt, a_args...);
    }

    template<typename... Args>
    inline void Message(const char* a_fmt, Args... a_args)
    {
        if (CheckLogLevel(LogLevel::Message))
            Write(a_fmt, a_args...);
    }

    template<typename... Args>
    inline void Warning(const char* a_fmt, Args... a_args)
    {
        if (CheckLogLevel(LogLevel::Warning))
            Write(a_fmt, a_args...);
    }

    template<typename... Args>
    inline void Error(const char* a_fmt, Args... a_args)
    {
        if (CheckLogLevel(LogLevel::Error))
            Write(a_fmt, a_args...);
    }

    template<typename... Args>
    inline void FatalError(const char* a_fmt, Args... a_args) {
        Write(a_fmt, a_args...);
    }

    inline void SetWriteCallback(onWriteCallback_t a_func) {
        m_onWriteCallback = a_func;
    }

    static LogLevel TranslateLogLevel(const std::string& a_level);

private:

    typedef std::unordered_map<std::string, LogLevel> logLevelMap_t;

    inline bool CheckLogLevel(LogLevel a_logLevel) const {
        return Enum::Underlying(m_logLevel) >= Enum::Underlying(a_logLevel);
    }

    template<typename... Args>
    void Write(const char* fmt, Args... a_args)
    {
        std::unique_ptr<char[]> buffer(new char[m_bufferSize]);

        _snprintf_s(buffer.get(), m_bufferSize, _TRUNCATE, fmt, a_args...);

        try
        {
            IScopedCriticalSection _(&m_criticalSection);

            if (m_ofstream.is_open())
                m_ofstream << buffer.get() << std::endl;
        }
        catch (...)
        {
        }

        if (m_onWriteCallback != nullptr)
            m_onWriteCallback(buffer.get());
    }

    std::ofstream m_ofstream;
    ICriticalSection m_criticalSection;
    LogLevel m_logLevel;
    size_t m_bufferSize;
    onWriteCallback_t m_onWriteCallback;

    static logLevelMap_t m_logLevelMap;
};

extern Logger gLogger;

class ILog
{
public:
    template<typename... Args>
    inline void Debug(const char* a_fmt, Args... a_args) const
    {
        gLogger.Debug(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    inline void Message(const char* a_fmt, Args... a_args) const
    {
        gLogger.Message(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    inline void Warning(const char* a_fmt, Args... a_args) const
    {
        gLogger.Warning(FormatString(a_fmt, "WARNING").c_str(), a_args...);
    }

    template<typename... Args>
    inline void Error(const char* a_fmt, Args... a_args) const
    {
        gLogger.Error(FormatString(a_fmt, "ERROR").c_str(), a_args...);
    }

    template<typename... Args>
    inline void FatalError(const char* a_fmt, Args... a_args) const
    {
        gLogger.FatalError(FormatString(a_fmt, "FATAL").c_str(), a_args...);
    }

    inline void LogPatchBegin(const char* a_id) const
    {
        gLogger.Debug(FormatString("[Patch] [%s] Writing..").c_str(), a_id);
    }

    inline void LogPatchEnd(const char* a_id) const
    {
        gLogger.Debug(FormatString("[Patch] [%s] OK").c_str(), a_id);
    }

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
};
