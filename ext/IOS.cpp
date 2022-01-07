#include "IOS.h"

std::size_t GetAllocGranularity()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return static_cast<std::size_t>(info.dwAllocationGranularity);
}

namespace WinApi
{
	void MessageBoxError(const char* a_caption, const char* a_message)
	{
		::MessageBoxA(NULL, a_message, a_caption, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
	}

	void MessageBoxErrorLog(const char* a_caption, const char* a_message)
	{
		::MessageBoxA(NULL, a_message, a_caption, MB_OK | MB_ICONERROR | MB_SYSTEMMODAL);
		gLog.Error("%s", a_message);
	}

	bool get_windows_path(std::filesystem::path& a_out)
	{
		TCHAR buffer[MAX_PATH];
		auto r = GetWindowsDirectoryA(buffer, MAX_PATH);

		if (r == 0 || r == MAX_PATH)
		{
			return false;
		}
		else
		{
			try
			{
				a_out = buffer;

				return true;
			}
			catch (const std::exception&)
			{
				return false;
			}
		}
	}
}