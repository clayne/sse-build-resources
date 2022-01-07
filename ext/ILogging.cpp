#include "ILogging.h"

const ILog::logLevelMap_t ILog::m_logLevelMap = {
	{ "Debug", LogLevel::Debug },
	{ "Message", LogLevel::Message },
	{ "Warning", LogLevel::Warning },
	{ "Error", LogLevel::Error },
	{ "Fatal", LogLevel::FatalError }
};

LogLevel ILog::TranslateLogLevel(const std::string& a_level)
{
	auto it = m_logLevelMap.find(a_level);
	return it != m_logLevelMap.end() ?
               it->second :
               LogLevel::Message;
}

const char* ILog::GetLogLevelString(LogLevel a_level)
{
	switch (a_level)
	{
	case LogLevel::Debug:
		return "Debug";
	case LogLevel::Message:
		return "Message";
	case LogLevel::Warning:
		return "Warning";
	case LogLevel::Error:
		return "Error";
	case LogLevel::FatalError:
		return "Fatal";
	default:
		return nullptr;
	}
}

BackLog::Entry::Entry(const LoggerMessageEvent& a_event) :
	m_level(a_event.level)
{
	if (a_event.prefix)
	{
		m_data += a_event.prefix;
	}

	m_data += a_event.message;
}

void BackLog::Add(const LoggerMessageEvent& a_event)
{
	IScopedLock _(m_lock);

	if (!m_limit)
	{
		return;
	}

	m_data.emplace_back(a_event);

	trim();
}

void BackLog::SetLimit(size_type a_limit)
{
	IScopedLock _(m_lock);

	m_limit = std::max<size_type>(a_limit, 1);

	trim();
}

void BackLog::Clear()
{
	IScopedLock _(m_lock);

	m_data.clear();
}

void BackLog::trim()
{
	auto it = m_data.begin();

	while (it != m_data.end() &&
	       m_data.size() > m_limit)
	{
		it = m_data.erase(it);
	}
}
