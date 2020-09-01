#include <filesystem>

#include <ShlObj.h>

Logger::Logger(size_t a_bufferSize) :
    m_logLevel(LogLevel::Message),
    m_bufferSize(a_bufferSize),
    m_onWriteCallback(nullptr)
{
}

Logger::Logger(LogLevel a_logLevel, size_t a_bufferSize) :
    m_logLevel(a_logLevel),
    m_bufferSize(a_bufferSize),
    m_onWriteCallback(nullptr)
{
}

Logger::~Logger() noexcept
{
    try
    {
        Close();
    }
    catch (...)
    {
    }
}

bool Logger::Open(const char* a_fname)
{
    return Open(nullptr, a_fname);
}

bool Logger::OpenRelative(int a_fid, const char* a_fname)
{
    std::unique_ptr<char> path(new char[MAX_PATH]);

    HRESULT hr = SHGetFolderPathA(
        nullptr,
        a_fid | CSIDL_FLAG_CREATE,
        nullptr,
        SHGFP_TYPE_CURRENT,
        path.get());

    if (!SUCCEEDED(hr))
        return false;

    return Open(path.get(), a_fname);
}

bool Logger::Open(const char* a_basepath, const char* a_fname)
{
    namespace fs = std::filesystem;

    try
    {
        if (m_ofstream.is_open())
            return false;

        fs::path fullpath;

        if (a_basepath != nullptr)
        {
            fullpath = a_basepath;

            if (!fs::exists(fullpath)) {
                if (!fs::create_directories(fullpath))
                    return false;
            }
            else if (!fs::is_directory(fullpath))
                return false;

            fullpath /= a_fname;
        }
        else {
            fullpath = a_fname;
        }

        m_ofstream.open(fullpath,
            std::ios_base::trunc |
            std::ios_base::out);

        if (!m_ofstream.is_open())
            return false;

        return true;
    }
    catch (...) {
        return false;
    }
}

void Logger::Close()
{
    if (m_ofstream.is_open())
        m_ofstream.close();
}

Logger::logLevelMap_t Logger::m_logLevelMap = {
    {"debug", LogLevel::Debug},
    {"message", LogLevel::Message},
    {"warning", LogLevel::Warning},
    {"error", LogLevel::Error},
    {"fatal", LogLevel::FatalError}
};

Logger::LogLevel Logger::TranslateLogLevel(const std::string& a_level)
{
    auto it = m_logLevelMap.find(a_level);
    if (it != m_logLevelMap.end())
        return it->second;
    else
        return LogLevel::Message;
}

Logger gLogger;