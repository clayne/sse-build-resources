#include "Patching.h"

namespace Patching
{
	void safe_write(uintptr_t addr, const void* data, size_t len)
	{
		DWORD oldProtect;
		ASSERT(VirtualProtect(reinterpret_cast<void*>(addr), len, PAGE_EXECUTE_READWRITE, std::addressof(oldProtect)));
		std::memcpy(reinterpret_cast<void*>(addr), data, len);
		ASSERT(VirtualProtect(reinterpret_cast<void*>(addr), len, oldProtect, &oldProtect));
	}

	void safe_memset(uintptr_t addr, int val, size_t len)
	{
		DWORD oldProtect;
		ASSERT(VirtualProtect(reinterpret_cast<void*>(addr), len, PAGE_EXECUTE_READWRITE, std::addressof(oldProtect)));
		std::memset(reinterpret_cast<void*>(addr), val, len);
		ASSERT(VirtualProtect(reinterpret_cast<void*>(addr), len, oldProtect, &oldProtect));
	}

	bool validate_mem(uintptr_t addr, const void* data, size_t len)
	{
		return std::memcmp(reinterpret_cast<void*>(addr), data, len) == 0;
	}

}