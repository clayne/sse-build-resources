#pragma once

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
        gLog.Debug("[Patch] [%s] Writing..", a_id);
    }

    SKMP_FORCEINLINE void LogPatchEnd(const char* a_id) const
    {
        gLog.Debug("[Patch] [%s] OK", a_id);
    }

    typedef stl::iunordered_map<std::string, IDebugLog::LogLevel> logLevelMap_t;
    static logLevelMap_t m_logLevelMap;

    static IDebugLog::LogLevel TranslateLogLevel(const std::string& a_level);

    FN_NAMEPROC("ILog")
private:

};


class BackLog
{
    typedef stl::vector<std::string> vec_t;

    using iterator = typename vec_t::iterator;
    using const_iterator = typename vec_t::const_iterator;

public:

    using size_type = vec_t::size_type;

    SKMP_FORCEINLINE BackLog(std::size_t a_limit) :
        m_limit(a_limit)
    {
    }

    [[nodiscard]] SKMP_FORCEINLINE const_iterator begin() const noexcept {
        return m_data.begin();
    }

    [[nodiscard]] SKMP_FORCEINLINE const_iterator end() const noexcept {
        return m_data.end();
    }

    SKMP_FORCEINLINE void Lock() noexcept {
        m_lock.Enter();
    }

    SKMP_FORCEINLINE void Unlock() noexcept {
        m_lock.Leave();
    }

    [[nodiscard]] SKMP_FORCEINLINE auto& GetLock() noexcept {
        return m_lock;
    }

    SKMP_FORCEINLINE void Add(const char* a_string);

    SKMP_FORCEINLINE void SetLimit(size_type a_limit);

private:
    ICriticalSection m_lock;

    vec_t m_data;
    size_type m_limit;
};

void BackLog::Add(const char* a_string)
{
    IScopedCriticalSection _(std::addressof(m_lock));

    m_data.emplace_back(a_string);
    if (m_data.size() > m_limit)
        m_data.erase(m_data.begin());
}

void BackLog::SetLimit(size_type a_limit)
{
    IScopedCriticalSection _(std::addressof(m_lock));

    m_limit = std::max<size_type>(a_limit, 1);

    while (m_data.size() > m_limit)
        m_data.erase(m_data.begin());

    m_data.shrink_to_fit();
}
