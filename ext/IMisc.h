#pragma once

#include <type_traits>

namespace stl
{
	template <class T>
	constexpr typename std::underlying_type<T>::type underlying(T a_value) noexcept
	{
		return static_cast<typename std::underlying_type<T>::type>(a_value);
	}

	template <class T>
	constexpr typename std::underlying_type<T>::type* underlying(T* a_ptr) noexcept
	{
		return reinterpret_cast<typename std::underlying_type<T>::type*>(a_ptr);
	}

	template <class T>
	using is_scoped = std::integral_constant<bool, !std::is_convertible_v<T, int> && std::is_enum_v<T>>;

	template <
		class T,
		class = std::enable_if_t<std::is_enum_v<T>, void>>
	struct flag
	{
		flag() = delete;

		inline flag(T const a_rhs) noexcept :
			value(a_rhs)
		{
		}

		inline flag& operator=(T const a_rhs) noexcept
		{
			value = a_rhs;
			return *this;
		}

		inline constexpr bool test(T const a_rhs) const noexcept
		{
			return (value & a_rhs) == a_rhs;
		}

		inline constexpr bool test_any(T const a_rhs) const noexcept
		{
			return (value & a_rhs) != static_cast<T>(0);
		}

		inline constexpr void set(T const a_rhs) noexcept
		{
			value |= a_rhs;
		}

		inline constexpr void toggle(T const a_rhs) noexcept
		{
			value ^= a_rhs;
		}

		inline constexpr void lshift(std::uint32_t a_offset) noexcept
		{
			value <<= a_offset;
		}

		inline constexpr void rshift(std::uint32_t a_offset) noexcept
		{
			value >>= a_offset;
		}

		inline constexpr void clear(T const a_rhs) noexcept
		{
			value &= ~a_rhs;
		}

		inline constexpr operator T() const noexcept
		{
			return value;
		}

		T value;
	};

	enum class flag_test_dummy : std::uint32_t
	{
	};

	static_assert(offsetof(flag<flag_test_dummy>, value) == 0);
	static_assert(sizeof(flag<flag_test_dummy>) == sizeof(std::underlying_type_t<flag_test_dummy>));
}

#define DEFINE_ENUM_CLASS_BITWISE(x)                                             \
	inline constexpr x operator|(x a_lhs, x a_rhs) noexcept                      \
	{                                                                            \
		return static_cast<x>(stl::underlying(a_lhs) | stl::underlying(a_rhs));  \
	}                                                                            \
	inline constexpr x operator&(x a_lhs, x a_rhs) noexcept                      \
	{                                                                            \
		return static_cast<x>(stl::underlying(a_lhs) & stl::underlying(a_rhs));  \
	}                                                                            \
	inline constexpr x operator^(x a_lhs, x a_rhs) noexcept                      \
	{                                                                            \
		return static_cast<x>(stl::underlying(a_lhs) ^ stl::underlying(a_rhs));  \
	}                                                                            \
	inline constexpr x operator~(x a_lhs) noexcept                               \
	{                                                                            \
		return static_cast<x>(~stl::underlying(a_lhs));                          \
	}                                                                            \
	inline constexpr x operator<<(x a_lhs, std::uint32_t a_offset) noexcept      \
	{                                                                            \
		return static_cast<x>(stl::underlying(a_lhs) << a_offset);               \
	}                                                                            \
	inline constexpr x operator>>(x a_lhs, std::uint32_t a_offset) noexcept      \
	{                                                                            \
		return static_cast<x>(stl::underlying(a_lhs) >> a_offset);               \
	}                                                                            \
	inline constexpr x& operator>>=(x& a_lhs, std::uint32_t a_offset) noexcept   \
	{                                                                            \
		a_lhs = static_cast<x>(stl::underlying(a_lhs) >> a_offset);              \
		return a_lhs;                                                            \
	}                                                                            \
	inline constexpr x& operator<<=(x& a_lhs, std::uint32_t a_offset) noexcept   \
	{                                                                            \
		a_lhs = static_cast<x>(stl::underlying(a_lhs) << a_offset);              \
		return a_lhs;                                                            \
	}                                                                            \
	inline constexpr x& operator|=(x& a_lhs, x a_rhs) noexcept                   \
	{                                                                            \
		a_lhs = static_cast<x>(stl::underlying(a_lhs) | stl::underlying(a_rhs)); \
		return a_lhs;                                                            \
	}                                                                            \
	inline constexpr x& operator&=(x& a_lhs, x a_rhs) noexcept                   \
	{                                                                            \
		a_lhs = static_cast<x>(stl::underlying(a_lhs) & stl::underlying(a_rhs)); \
		return a_lhs;                                                            \
	}                                                                            \
	inline constexpr x& operator^=(x& a_lhs, x a_rhs) noexcept                   \
	{                                                                            \
		a_lhs = static_cast<x>(stl::underlying(a_lhs) ^ stl::underlying(a_rhs)); \
		return a_lhs;                                                            \
	}
