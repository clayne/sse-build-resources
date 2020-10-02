#include "ILogging.h"

IDebugLog gLog;

ILog::logLevelMap_t ILog::m_logLevelMap = {
    {"debug", IDebugLog::LogLevel::Debug},
    {"message", IDebugLog::LogLevel::Message},
    {"warning", IDebugLog::LogLevel::Warning},
    {"error", IDebugLog::LogLevel::Error},
    {"fatal", IDebugLog::LogLevel::FatalError}
};

IDebugLog::LogLevel ILog::TranslateLogLevel(const std::string& a_level)
{
    auto it = m_logLevelMap.find(a_level);
    if (it != m_logLevelMap.end())
        return it->second;
    else
        return IDebugLog::LogLevel::Message;
}
