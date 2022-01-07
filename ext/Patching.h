#pragma once

namespace Patching
{
	void safe_write(uintptr_t addr, const void* data, size_t len);
	void safe_memset(uintptr_t addr, int val, size_t len);
	bool validate_mem(uintptr_t addr, const void* data, size_t len);

	template <std::size_t _Size>
	constexpr bool validate_mem(uintptr_t addr, const std::uint8_t (&a_data)[_Size])
	{
		return validate_mem(addr, reinterpret_cast<const void*>(a_data), _Size);
	}

	template <
		class T,
		class = std::enable_if_t<std::is_fundamental_v<T>>>
	void safe_write(uintptr_t addr, T val)
	{
		safe_write(addr, reinterpret_cast<const void*>(std::addressof(val)), sizeof(val));
	}

	template <std::size_t _Size>
	void safe_write(uintptr_t a_addr, const std::uint8_t (&a_data)[_Size])
	{
		safe_write(a_addr, reinterpret_cast<const void*>(a_data), _Size);
	}
}