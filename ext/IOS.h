#pragma once

#include "ILogging.h"

std::size_t GetAllocGranularity();

namespace WinApi
{
	void MessageBoxError(const char* a_caption, const char* a_message);
	void MessageBoxErrorLog(const char* a_caption, const char* a_message);

	template <class... Args>
	void MessageBoxErrorFmt(const char* a_caption, const char* a_fmt, Args... a_args)
	{
		char buf[260];
		_snprintf_s(buf, _TRUNCATE, a_fmt, std::forward<Args>(a_args)...);

		MessageBoxA(NULL, buf, a_caption, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}

	template <class... Args>
	void MessageBoxErrorFmtLog(const char* a_caption, const char* a_fmt, Args... a_args)
	{
		MessageBoxErrorFmt(a_caption, a_fmt, std::forward<Args>(a_args)...);
		gLog.Error(a_fmt, std::forward<Args>(a_args)...);
	}
}