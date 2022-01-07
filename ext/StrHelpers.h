#pragma once

#include <codecvt>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#ifdef UNICODE
#	define STD_STRING std::std::wstring
#	define STD_OFSTREAM std::wofstream
#	define STD_OSSTREAM std::wostringstream
#	define STD_ISSTREAM std::wistringstream
#	define STD_SSTREAM std::wstringstream
#	define STD_TOSTR std::to_wstring

#	define VSPRINTF _vsnwprintf_s
#	define SPRINTF _snwprintf_s
#	define STRCMP _wcsicmp
#	define FSOPEN _wfsopen
#	define MKDIR _wmkdir
#	define FPUTS fputws
#else
#	define STD_STRING std::string
#	define STD_OFSTREAM std::ofstream
#	define STD_OSSTREAM std::ostringstream
#	define STD_ISSTREAM std::istringstream
#	define STD_SSTREAM std::stringstream
#	define STD_TOSTR std::to_string

#	define VSPRINTF _vsnprintf_s
#	define SPRINTF _snprintf_s
#	define STRCMP _stricmp
#	define FSOPEN _fsopen
#	define MKDIR _mkdir
#	define FPUTS fputs
#endif

namespace StrHelpers
{
	using wsconv_t = std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>;
	extern wsconv_t g_strconverter;

	std::string ToString(const std::wstring& wstr);
	std::wstring ToWString(const std::string& str);

	template <class Tc, class To>
	void SplitString(
		const std::basic_string<Tc>& a_in,
		char a_delim,
		std::vector<To>& a_out,
		bool a_skipEmpty = false,
		bool a_hex = false)
	{
		std::basic_istringstream<Tc> ssi(a_in);
		std::basic_string<Tc> item;

		while (std::getline(ssi, item, a_delim))
		{
			if (a_skipEmpty && item.empty())
			{
				continue;
			}

			std::basic_stringstream<Tc> convss;
			To iv{};

			try
			{
				if (a_hex)
				{
					convss << std::hex;
				}

				convss << item;
				convss >> iv;
			}
			catch (...)
			{
				continue;
			}

			a_out.push_back(std::move(iv));
		}
	}

#ifdef UNICODE
	SKMP_FORCEINLINE std::wstring ToNative(const std::string& str)
	{
		return g_strconverter.from_bytes(str);
	}

	SKMP_FORCEINLINE constexpr const std::wstring& ToNative(const std::wstring& str) noexcept
	{
		return str;
	}

	SKMP_FORCEINLINE constexpr std::string StrToStr(const std::wstring& str) noexcept
	{
		return g_strconverter.to_bytes(str);
	}

#else
	SKMP_FORCEINLINE std::string ToNative(const std::wstring& wstr)
	{
		return g_strconverter.to_bytes(wstr);
	}

	SKMP_FORCEINLINE constexpr const std::string& ToNative(std::string& str) noexcept
	{
		return str;
	}

	SKMP_FORCEINLINE constexpr const std::string& StrToStr(const std::string& str) noexcept
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

	SKMP_FORCEINLINE bool iequal(const std::string& a_lhs, const std::string& a_rhs)
	{
		if (a_lhs.size() != a_rhs.size())
			return false;

		return _stricmp(a_lhs.c_str(), a_rhs.c_str()) == 0;
	}

	SKMP_FORCEINLINE std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	SKMP_FORCEINLINE std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	SKMP_FORCEINLINE std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		return ltrim(rtrim(str, chars), chars);
	}

	template <std::size_t _Len>
	std::size_t strlen(const char (&a_string)[_Len])
	{
		return ::strnlen(a_string, _Len);
	}

}
