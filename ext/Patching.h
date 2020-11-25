#pragma once

namespace Patching
{
	void safe_write(uintptr_t addr, const void* data, size_t len);
	void safe_memset(uintptr_t addr, int val, size_t len);
	bool validate_mem(uintptr_t addr, const void* data, size_t len);

	template <typename T>
	SKMP_FORCEINLINE void safe_write(uintptr_t addr, T val)
	{
		safe_write(addr, reinterpret_cast<const void*>(&val), sizeof(val));
	}
}