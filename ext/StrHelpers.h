#pragma once

#include <string>
#include <sstream>
#include <locale>
#include <codecvt>
#include <vector>

#ifdef UNICODE
#define STD_STRING      std::std::wstring
#define STD_OFSTREAM    std::wofstream
#define STD_OSSTREAM    std::wostringstream
#define STD_ISSTREAM    std::wistringstream
#define STD_SSTREAM     std::wstringstream
#define STD_TOSTR       std::to_wstring

#define VSPRINTF        _vsnwprintf_s
#define SPRINTF         _snwprintf_s
#define STRCMP          _wcsicmp
#define FSOPEN          _wfsopen
#define MKDIR           _wmkdir
#define FPUTS           fputws
#else
#define STD_STRING      std::string
#define STD_OFSTREAM    std::ofstream
#define STD_OSSTREAM    std::ostringstream
#define STD_ISSTREAM    std::istringstream
#define STD_SSTREAM     std::stringstream
#define STD_TOSTR       std::to_string

#define VSPRINTF        _vsnprintf_s
#define SPRINTF         _snprintf_s
#define STRCMP          _stricmp
#define FSOPEN          _fsopen
#define MKDIR           _mkdir
#define FPUTS           fputs
#endif

namespace StrHelpers
{
    typedef std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> wsconv_t;
    static inline wsconv_t strconverter;

    SKMP_FORCEINLINE std::string ToString(const std::wstring& wstr)
    {
        return strconverter.to_bytes(wstr);
    }

    SKMP_FORCEINLINE std::wstring ToWString(const std::string& str)
    {
        return strconverter.from_bytes(str);
    }

    SKMP_FORCEINLINE void
        SplitString(const std::wstring& s, wchar_t delim, stl::vector<std::wstring>& elems)
    {
        std::wistringstream ss(s);
        std::wstring item;

        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }

    template <typename T>
    void SplitString(const std::wstring& s, wchar_t delim, stl::vector<T>& elems)
    {
        stl::vector<std::wstring> tmp;
        SplitString(s, delim, tmp);

        if (tmp.size())
        {
            T iv;
            wstringstream oss;
            for (const auto& e : tmp) {
                oss << e;
                oss >> iv;
                oss.clear();
                elems.push_back(iv);
            }
        }
    }

    SKMP_FORCEINLINE void
        SplitString(const std::string& s, char delim, stl::vector<std::string>& elems)
    {
        std::istringstream ss(s);
        std::string item;

        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }

    template <typename T>
    void SplitString(const std::string& s, char delim, stl::vector<T>& elems)
    {
        stl::vector<std::string> tmp;
        SplitString(s, delim, tmp);

        if (tmp.size())
        {
            T iv;
            std::stringstream oss;
            for (const auto& e : tmp) {
                oss << e;
                oss >> iv;
                oss.clear();
                elems.push_back(iv);
            }
        }
    }

#ifdef UNICODE
    SKMP_FORCEINLINE std::wstring ToNative(const std::string& str)
    {
        return strconverter.from_bytes(str);
    }

    SKMP_FORCEINLINE const std::wstring& ToNative(const std::wstring& str)
    {
        return str;
    }

    SKMP_FORCEINLINE std::string StrToStr(const std::wstring& str)
    {
        return strconverter.to_bytes(str);
    }

#else
    SKMP_FORCEINLINE std::string ToNative(const std::wstring& wstr)
    {
        return strconverter.to_bytes(wstr);
    }

    SKMP_FORCEINLINE const std::string& ToNative(std::string& str)
    {
        return str;
    }

    SKMP_FORCEINLINE const std::string& StrToStr(const std::string& str)
    {
        return str;
    }

#endif

    SKMP_FORCEINLINE const int icompare(const std::string& a_lhs, const std::string& a_rhs)
    {
        return _stricmp(a_lhs.c_str(), a_rhs.c_str());
    }

    SKMP_FORCEINLINE const int icompare(const char* a_lhs, const char* a_rhs)
    {
        return _stricmp(a_lhs, a_rhs);
    }

}
