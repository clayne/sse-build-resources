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
        FatalError,
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
    void Debug(const char* a_fmt, Args... a_args)
    {
        if (!CheckLogLevel(LogLevel::Debug))
            return;

        Write(a_fmt, a_args...);
    }

    template<typename... Args>
    void Message(const char* a_fmt, Args... a_args)
    {
        if (!CheckLogLevel(LogLevel::Message))
            return;

        Write(a_fmt, a_args...);
    }

    template<typename... Args>
    void Warning(const char* a_fmt, Args... a_args)
    {
        if (!CheckLogLevel(LogLevel::Warning))
            return;

        Write(a_fmt, a_args...);
    }

    template<typename... Args>
    void Error(const char* a_fmt, Args... a_args)
    {
        if (!CheckLogLevel(LogLevel::Error))
            return;

        Write(a_fmt, a_args...);
    }

    template<typename... Args>
    void FatalError(const char* a_fmt, Args... a_args) {
        Write(a_fmt, a_args...);
    }

    inline void SetWriteCallback(onWriteCallback_t a_func) {
        m_onWriteCallback = a_func;
    }

    static LogLevel TranslateLogLevel(const std::string& a_level);

private:

    typedef std::unordered_map<std::string, LogLevel> logLevelMap_t;

    inline bool CheckLogLevel(LogLevel a_logLevel) {
        return Enum::Underlying(m_logLevel) >= Enum::Underlying(a_logLevel);
    }

    template<typename... Args>
    void Write(const char* fmt, Args... a_args)
    {
        std::unique_ptr<char[]> buffer(new char[m_bufferSize]);

        _snprintf_s(buffer.get(), m_bufferSize, _TRUNCATE, fmt, a_args...);

        m_criticalSection.Enter();

        try
        {
            if (m_ofstream.is_open())
                m_ofstream << buffer.get() << std::endl;
        }
        catch (...)
        {
        }

        m_criticalSection.Leave();

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
    void Debug(const char* a_fmt, Args... a_args)
    {
        gLogger.Debug(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    void Message(const char* a_fmt, Args... a_args)
    {
        gLogger.Message(FormatString(a_fmt).c_str(), a_args...);
    }

    template<typename... Args>
    void Warning(const char* a_fmt, Args... a_args)
    {
        gLogger.Warning(FormatString(a_fmt, "WARNING").c_str(), a_args...);
    }

    template<typename... Args>
    void Error(const char* a_fmt, Args... a_args)
    {
        gLogger.Error(FormatString(a_fmt, "ERROR").c_str(), a_args...);
    }

    template<typename... Args>
    void FatalError(const char* a_fmt, Args... a_args)
    {
        gLogger.FatalError(FormatString(a_fmt, "FATAL").c_str(), a_args...);
    }

    inline void LogPatchBegin(const char* a_id)
    {
        gLogger.Debug("[Patch] [%s] Writing..", a_id);
    }

    inline void LogPatchEnd(const char* a_id)
    {
        gLogger.Debug("[Patch] [%s] OK", a_id);
    }

    FN_NAMEPROC("ILog")
private:

    inline std::string FormatString(const char* a_fmt, const char* a_pfix = nullptr) {
        std::ostringstream _fmt;
        if (a_pfix != nullptr)
            _fmt << "<" << a_pfix << "> ";
        _fmt << "[" << ModuleName() << "] " << a_fmt;

        return _fmt.str();
    }
};
