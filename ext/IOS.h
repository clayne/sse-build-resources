#pragma once

std::size_t GetAllocGranularity();

namespace WinApi
{
    void MessageBoxError(const char* a_caption, const char* a_message);

	template <class... Args>
	void MessageBoxErrorFmt(const char* a_fmt, Args... a_args)
	{
		char buf[260];
		_snprintf_s(buf, _TRUNCATE, a_fmt, std::forward<Args>(a_args)...);

		MessageBoxA(NULL, buf, PLUGIN_NAME, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}
}