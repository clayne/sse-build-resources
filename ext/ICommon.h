#pragma once

#define FN_NAMEPROC(x)                                                                      \
	virtual const char* ModuleName() const noexcept { return x; };                          \
	virtual const char* LogPrefix() const noexcept { return "[" x "] "; };                  \
	virtual const char* LogPrefixWarning() const noexcept { return "<WARNING> [" x "] "; }; \
	virtual const char* LogPrefixError() const noexcept { return "<ERROR> [" x "] "; };     \
	virtual const char* LogPrefixFatal() const noexcept { return "<FATAL> [" x "] "; };

#define SKMP_FORCEINLINE __forceinline
#define SKMP_NOINLINE __declspec(noinline)
#define SKMP_ALIGN(x) __declspec(align(x))

#if defined(__AVX__) || defined(__AVX2__)
#	define SIMD_ALIGNMENT 32
#else
#	define SIMD_ALIGNMENT 16
#endif

#define SKMP_ALLOC_ALIGN SIMD_ALIGNMENT

#define SKMP_ALIGN_AUTO __declspec(align(SIMD_ALIGNMENT))

#include <exception>
#include <string>
#include <xstddef>

#include "STLCommon.h"

namespace except
{
	class descriptor
	{
	public:
		descriptor() :
			m_desc(std::exception().what())
		{
		}

		descriptor(std::exception const& a_rhs)
		{
			m_desc = a_rhs.what();
		}

		descriptor& operator=(std::exception const& a_rhs)
		{
			m_desc = a_rhs.what();
			return *this;
		}

		descriptor& operator=(const char* a_desc)
		{
			m_desc = a_desc;
			return *this;
		}

		inline constexpr const char* what() const noexcept
		{
			return m_desc.c_str();
		}

	private:
		std::string m_desc;
	};
}

template <class T>
class SetObjectWrapper
{
public:
	using held_type = T;

	SetObjectWrapper() = default;

	explicit SetObjectWrapper(const SetObjectWrapper<T>& a_rhs) :
		m_set(a_rhs.m_set),
		m_item(a_rhs.m_item)
	{
	}

	SetObjectWrapper(const T& a_rhs) :
		m_set(true),
		m_item(a_rhs)
	{
	}

	explicit SetObjectWrapper(SetObjectWrapper<T>&& a_rhs) :
		m_set(a_rhs.m_set),
		m_item(std::move(a_rhs.m_item))
	{
	}

	SetObjectWrapper(T&& a_rhs) :
		m_set(true),
		m_item(std::move(a_rhs))
	{
	}

	template <class... Args>
	constexpr T& emplace(Args&&... a_args)
	{
		stl::construct_in_place(m_item, std::forward<Args>(a_args)...);
		m_set = true;

		return m_item;
	}

	template <class... Args>
	constexpr T& try_emplace(Args&&... a_args)
	{
		if (!m_set)
		{
			return emplace(std::forward<Args>(a_args)...);
		}
		else
		{
			return m_item;
		}
	}

	SetObjectWrapper<T>& operator=(const SetObjectWrapper<T>& a_rhs)
	{
		m_item = a_rhs.m_item;
		m_set = a_rhs.m_set;
		return *this;
	}

	SetObjectWrapper<T>& operator=(const T& a_rhs)
	{
		if (!m_set)
		{
			stl::construct_in_place(m_item, a_rhs);
			m_set = true;
		}
		else
		{
			m_item = a_rhs;
		}
		
		return *this;
	}

	SetObjectWrapper<T>& operator=(SetObjectWrapper<T>&& a_rhs)
	{
		m_item = std::move(a_rhs.m_item);
		m_set = a_rhs.m_set;
		return *this;
	}

	SetObjectWrapper<T>& operator=(T&& a_rhs)
	{
		if (!m_set)
		{
			stl::construct_in_place(m_item, std::move(a_rhs));
			m_set = true;
		}
		else
		{
			m_item = std::move(a_rhs);
		}
		return *this;
	}

	inline constexpr bool operator==(const T& a_rhs) const noexcept
	{
		return m_set && m_item == a_rhs;
	}

	inline constexpr bool operator!=(const T& a_rhs) const noexcept
	{
		return !m_set || m_item != a_rhs;
	}

	inline constexpr void clear() noexcept
	{
		m_set = false;
	}

	[[nodiscard]] inline constexpr T& clear_and_get() noexcept
	{
		m_set = false;
		return m_item;
	}

	inline constexpr void reset() noexcept
	{
		stl::construct_in_place(m_item);
		m_set = false;
	}

	inline constexpr void mark(bool a_switch) noexcept
	{
		m_set = a_switch;
	}

	[[nodiscard]] inline constexpr const T& get() const noexcept
	{
		return m_item;
	}

	[[nodiscard]] inline constexpr T& get() noexcept
	{
		return m_item;
	}

	[[nodiscard]] inline constexpr const T& operator*() const noexcept
	{
		return m_item;
	}

	[[nodiscard]] inline constexpr T& operator*() noexcept
	{
		return m_item;
	}

	[[nodiscard]] inline constexpr const T* operator->() const noexcept
	{
		return std::addressof(m_item);
	}

	[[nodiscard]] inline constexpr T* operator->() noexcept
	{
		return std::addressof(m_item);
	}

	[[nodiscard]] inline constexpr bool has() const noexcept
	{
		return m_set;
	}

	[[nodiscard]] inline constexpr explicit operator bool() const noexcept
	{
		return m_set;
	}

protected:
	T m_item{};
	bool m_set{ false };
};

#include "AddressLibrary.h"
#include "GameHandles.h"
#include "Hash.h"
#include "IHook.h"
#include "ILogging.h"
#include "IMisc.h"
#include "IRandom.h"
#include "Math.h"
#include "Mem.h"
#include "Patching.h"
#include "PerfCounter.h"
#include "RTTI.h"
#include "STL.h"
#include "Threads.h"
#include "stl_stdio.h"