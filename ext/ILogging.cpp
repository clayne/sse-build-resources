#include "ILogging.h"

IDebugLog gLog;

ILog::logLevelMap_t ILog::m_logLevelMap = {
    {"debug", IDebugLog::LogLevel::Debug},
    {"verbose", IDebugLog::LogLevel::Verbose},
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


BackLog::LogString::LogString(const LoggerMessageEvent& a_event) noexcept
{
    std::size_t msgSize = strlen(a_event.a_message);
    std::size_t prefixSize = a_event.a_prefix ? std::strlen(a_event.a_prefix) : 0;
    std::size_t totalSize = prefixSize + msgSize + 1;

    m_data = static_cast<char*>(_mm_malloc(totalSize, 32));

    if (!a_event.a_prefix) {
        std::memcpy(m_data, a_event.a_message, totalSize);
    }
    else
    {
        if (prefixSize > 0)
            std::memcpy(m_data, a_event.a_prefix, prefixSize);

        std::memcpy(&m_data[prefixSize], a_event.a_message, msgSize + 1);
    }
}

void BackLog::Add(const LoggerMessageEvent& a_event)
{
    IScopedLock _(m_lock);

    m_data.emplace_back(a_event);

    if (m_data.size() > m_limit)
        m_data.erase(m_data.begin());
}

void BackLog::SetLimit(size_type a_limit)
{
    IScopedLock _(m_lock);

    m_limit = std::max<size_type>(a_limit, 1);

    while (m_data.size() > m_limit)
        m_data.erase(m_data.begin());

    m_data.shrink_to_fit();
}
