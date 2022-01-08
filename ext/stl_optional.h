#pragma once

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
#	include <boost/serialization/access.hpp>
#	include "boost_macros.h"
#endif

namespace stl
{
	template <class T>
	class optional
	{
#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
		friend class boost::serialization::access;
#endif

	public:

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
		enum Serialization : unsigned int
		{
			DataVersion1 = 1
		};
#endif

		using held_type = T;

		optional() = default;

		explicit optional(const optional& a_rhs) :
			m_set(a_rhs.m_set),
			m_item(a_rhs.m_item)
		{
		}

		optional(const T& a_rhs) :
			m_set(true),
			m_item(a_rhs)
		{
		}

		explicit optional(optional&& a_rhs) :
			m_set(a_rhs.m_set),
			m_item(std::move(a_rhs.m_item))
		{
		}

		optional(T&& a_rhs) :
			m_set(true),
			m_item(std::move(a_rhs))
		{
		}

		template <class... Args>
		constexpr T& insert(Args&&... a_args)
		{
			m_item = T(std::forward<Args>(a_args)...);
			m_set = true;

			return m_item;
		}

		template <class... Args>
		constexpr T& try_insert(Args&&... a_args)
		{
			if (!m_set)
			{
				return insert(std::forward<Args>(a_args)...);
			}
			else
			{
				return m_item;
			}
		}

		optional& operator=(const optional& a_rhs)
		{
			m_item = a_rhs.m_item;
			m_set = a_rhs.m_set;
			return *this;
		}

		optional& operator=(const T& a_rhs)
		{
			m_item = a_rhs;
			m_set = true;
			return *this;
		}

		optional& operator=(optional&& a_rhs)
		{
			m_item = std::move(a_rhs.m_item);
			m_set = a_rhs.m_set;
			return *this;
		}

		optional& operator=(T&& a_rhs)
		{
			m_item = std::move(a_rhs);
			m_set = true;
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
			m_item = {};
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

	private:
#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
		template <class Archive>
		void serialize(Archive& ar, const unsigned int version)
		{
			ar& m_set;
			if (m_set)
			{
				ar& m_item;
			}
		}
#endif
	};

}

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
BOOST_CLASS_TEMPLATE_VERSION(
	class T,
	::stl::optional<T>,
	::stl::optional<T>::Serialization::DataVersion1);
#endif
