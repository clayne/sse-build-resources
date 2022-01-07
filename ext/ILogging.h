#pragma once

#include "STL.h"
#include "Threads.h"

class ILog
{
public:
	template <class... Args>
	SKMP_NOINLINE constexpr void Debug(const char* a_fmt, Args... a_args) const
	{
		if (gLog.CheckLogLevel(LogLevel::Debug))
		{
			gLog.Write(
				LogLevel::Debug,
				a_fmt,
				LogPrefix(),
				a_args...);
		}
	}

	template <class... Args>
	SKMP_NOINLINE constexpr void Message(const char* a_fmt, Args... a_args) const
	{
		if (gLog.CheckLogLevel(LogLevel::Debug))
		{
			gLog.Write(
				LogLevel::Debug,
				a_fmt,
				LogPrefix(),
				a_args...);
		}
	}

	template <class... Args>
	SKMP_NOINLINE constexpr void Warning(const char* a_fmt, Args... a_args) const
	{
		if (gLog.CheckLogLevel(LogLevel::Warning))
		{
			gLog.Write(
				LogLevel::Warning,
				a_fmt,
				LogPrefixWarning(),
				a_args...);
		}
	}

	template <class... Args>
	SKMP_NOINLINE constexpr void Error(const char* a_fmt, Args... a_args) const
	{
		if (gLog.CheckLogLevel(LogLevel::Error))
		{
			gLog.Write(
				LogLevel::Error,
				a_fmt,
				LogPrefixError(),
				a_args...);
		}
	}

	template <class... Args>
	SKMP_NOINLINE constexpr void FatalError(const char* a_fmt, Args... a_args) const
	{
		if (gLog.CheckLogLevel(LogLevel::FatalError))
		{
			gLog.Write(
				LogLevel::FatalError,
				a_fmt,
				LogPrefixFatal(),
				a_args...);
		}
	}

	SKMP_NOINLINE constexpr void LogPatchBegin(const char* a_id) const
	{
		Debug("[Patch] [%s] Writing..", a_id);
	}

	SKMP_NOINLINE constexpr void LogPatchEnd(const char* a_id) const
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

	FN_NAMEPROC("ILog");

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
		explicit Entry(Entry&&) = delete;

		Entry& operator=(const Entry&) = delete;
		Entry& operator=(Entry&&) = delete;

		inline constexpr const char* data() const noexcept
		{
			return m_data.data();
		}

		inline constexpr operator const auto &() const noexcept
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
	void trim();

	mutable WCriticalSection m_lock;

	storage_type m_data;
	size_type m_limit;
};
