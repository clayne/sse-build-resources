#pragma once

#if defined(SKMP_ENABLE_STRING_CACHE)

#	include <boost/serialization/split_member.hpp>
#	include <boost/serialization/string.hpp>
#	include <boost/serialization/version.hpp>

#	include <memory>
#	include <string>
#	include <unordered_set>

#	include "STLCommon.h"
#	include "Threads.h"

namespace string_cache
{
	class data_pool;

	template <class Tc = char>
	class icase_key
	{
		friend class data_pool;

	public:
		using atomic_type = std::atomic<std::uint64_t>;

		icase_key() = delete;

		icase_key(
			std::size_t a_hash,
			const std::basic_string<Tc>& a_value) :
			m_hash(a_hash),
			m_value(a_value)
		{
		}

		icase_key(
			std::size_t a_hash,
			std::basic_string<Tc>&& a_value) :
			m_hash(a_hash),
			m_value(std::move(a_value))
		{
		}

		icase_key(const icase_key&) = delete;
		icase_key(icase_key&&) = delete;
		icase_key& operator=(const icase_key&) = delete;
		icase_key& operator=(icase_key&&) = delete;

		[[nodiscard]] inline constexpr auto hash() const noexcept
		{
			return m_hash;
		}

		[[nodiscard]] inline constexpr const auto& value() const noexcept
		{
			return m_value;
		}

		[[nodiscard]] friend inline constexpr bool operator==(
			const icase_key& a_lhs,
			const icase_key& a_rhs) noexcept
		{
			if (a_lhs.hash() != a_rhs.hash())
			{
				return false;
			}

			auto& lhs_data = a_lhs.value();
			auto& rhs_data = a_rhs.value();

			if (lhs_data.size() != rhs_data.size())
			{
				return false;
			}

			if constexpr (std::is_same_v<Tc, char>)
			{
				return ::hash::stricmp(lhs_data.c_str(), rhs_data.c_str()) == 0;
			}
			else if constexpr (std::is_same_v<Tc, wchar_t>)
			{
				return _wcsicmp(lhs_data.c_str(), rhs_data.c_str()) == 0;
			}
			else
			{
				static_assert(false, "Not implemented");
			}
		}

		static inline constexpr std::size_t compute_hash(
			const std::basic_string<Tc>& a_value) noexcept;

	private:
		const std::size_t m_hash;
		const std::basic_string<Tc> m_value;

	public:
		[[nodiscard]] inline auto use_count() const noexcept
		{
			return m_refcount.load(std::memory_order_acquire);
		}

		inline void inc_refcount() const noexcept
		{
			m_refcount.fetch_add(1, std::memory_order_relaxed);
		}

		[[nodiscard]] inline auto dec_refcount() const noexcept
		{
			return m_refcount.fetch_sub(1, std::memory_order_acq_rel);
		}

	private:
		mutable atomic_type m_refcount{ 0 };

		static_assert(atomic_type::is_always_lock_free);
	};

	template <class Tc>
	inline constexpr std::size_t icase_key<Tc>::compute_hash(
		const std::basic_string<Tc>& a_value) noexcept
	{
		using namespace ::hash::fnv1;

		std::size_t hash = fnv_offset_basis;

		for (auto& e : a_value)
		{
			if constexpr (std::is_same_v<Tc, char>)
			{
				hash ^= static_cast<std::size_t>(::hash::toupper(e));
			}
			else if constexpr (std::is_same_v<Tc, wchar_t>)
			{
				hash ^= static_cast<std::size_t>(::hash::towupper(e));
			}
			else
			{
				static_assert(false, "Not implemented");
			}

			hash *= fnv_prime;
		}

		return hash;
	}

}

namespace std
{
	template <>
	struct hash<string_cache::icase_key<char>>
	{
		[[nodiscard]] inline constexpr auto operator()(
			string_cache::icase_key<char> const& a_arg) const noexcept
		{
			return a_arg.hash();
		}
	};
}

namespace stl
{
	class fixed_string;
};

namespace string_cache
{
	class data_pool
	{
		friend class stl::fixed_string;

		using container_type = std::unordered_set<icase_key<char>>;
		using value_type = typename container_type::value_type;

		struct stats_t
		{
			std::size_t count{ 0 };
			std::size_t map_usage{ 0 };
			std::size_t key_data_usage{ 0 };
			std::size_t string_usage{ 0 };

			std::size_t total_fixed_count{ 0 };
			std::size_t total_fixed_size{ 0 };

			std::size_t estimated_uncached_usage{ 0 };

			std::size_t total{ 0 };
			long double ratio{ 0 };
		};

	public:
		data_pool();

		[[nodiscard]] inline static auto size() noexcept
		{
			return m_Instance.m_data.size();
		}

		[[nodiscard]] static stats_t get_stats() noexcept;

	private:
		static const value_type& insert(
			std::size_t a_hash,
			const std::string& a_string);

		static const value_type& insert(
			std::size_t a_hash,
			std::string&& a_string);

		[[nodiscard]] inline constexpr static auto& get_lock() noexcept
		{
			return m_Instance.m_lock;
		}

		[[nodiscard]] inline constexpr static auto get_empty() noexcept
		{
			return m_Instance.m_icase_empty;
		}

		[[nodiscard]] inline constexpr static auto& get_data() noexcept
		{
			return m_Instance.m_data;
		}

		static void release(
			const value_type* a_ref) noexcept;

		template <bool _Lock>
		static void try_release(
			const value_type* a_ref) noexcept;

		FastSpinLock m_lock;

		const icase_key<char>* m_icase_empty;

		container_type m_data;

		static data_pool m_Instance;
	};
}

namespace stl
{
	// case insensitive
	class fixed_string
	{
		friend class boost::serialization::access;

	public:
		inline fixed_string::fixed_string() noexcept :
			m_ref(string_cache::data_pool::get_empty())
		{
		}

		inline fixed_string::fixed_string(std::nullptr_t) noexcept :
			m_ref(string_cache::data_pool::get_empty())
		{
		}

		fixed_string::~fixed_string() noexcept
		{
			try_release<true>();
		}

		fixed_string(const fixed_string& a_rhs) noexcept :
			m_ref(a_rhs.m_ref)
		{
			try_acquire();
		}

		inline fixed_string(fixed_string&& a_rhs) noexcept :
			m_ref(a_rhs.m_ref)
		{
			a_rhs.m_ref = string_cache::data_pool::get_empty();
		}

		fixed_string& operator=(const fixed_string& a_rhs) noexcept
		{
			if (this != std::addressof(a_rhs))
			{
				try_release<true>();
				m_ref = a_rhs.m_ref;
				try_acquire();
			}
			return *this;
		}

		fixed_string& operator=(fixed_string&& a_rhs) noexcept
		{
			if (this != std::addressof(a_rhs))
			{
				try_release<true>();
				m_ref = a_rhs.m_ref;
				a_rhs.m_ref = string_cache::data_pool::get_empty();
			}
			return *this;
		}

		fixed_string(const std::string& a_value) :
			m_ref(string_cache::data_pool::get_empty())
		{
			set_impl(a_value);
		}

		fixed_string(std::string&& a_value) :
			m_ref(string_cache::data_pool::get_empty())
		{
			set_impl(std::move(a_value));
		}

		fixed_string(const char* a_value) :
			m_ref(string_cache::data_pool::get_empty())
		{
			set(a_value);
		}

		fixed_string& operator=(const std::string& a_value)
		{
			set_impl(a_value);
			return *this;
		}

		fixed_string& operator=(std::string&& a_value)
		{
			set_impl(std::move(a_value));
			return *this;
		}

		fixed_string& operator=(const char* a_value)
		{
			set(a_value);
			return *this;
		}

		void clear() noexcept
		{
			try_release<true>();
			m_ref = string_cache::data_pool::get_empty();
		}

		[[nodiscard]] inline constexpr operator const auto &() const noexcept
		{
			return m_ref->value();
		}

		[[nodiscard]] inline constexpr const auto& operator*() const noexcept
		{
			return m_ref->value();
		}

		[[nodiscard]] inline constexpr auto hash() const noexcept
		{
			return m_ref->hash();
		}

		[[nodiscard]] inline constexpr const auto& get() const noexcept
		{
			return m_ref->value();
		}

		[[nodiscard]] inline constexpr auto data() const noexcept
		{
			return m_ref->value().data();
		}

		[[nodiscard]] inline constexpr auto c_str() const noexcept
		{
			return m_ref->value().c_str();
		}

		[[nodiscard]] inline constexpr auto size() const noexcept
		{
			return m_ref->value().size();
		}

		[[nodiscard]] inline constexpr auto empty() const noexcept
		{
			return m_ref->value().empty();
		}

		[[nodiscard]] inline constexpr bool less_equal_p(
			const fixed_string& a_rhs) const noexcept
		{
			return m_ref <= a_rhs.m_ref;
		}

		[[nodiscard]] inline constexpr bool less_p(
			const fixed_string& a_rhs) const noexcept
		{
			return m_ref < a_rhs.m_ref;
		}

	private:
		[[nodiscard]] inline constexpr auto ref() const noexcept
		{
			return m_ref;
		}

		[[nodiscard]] friend inline constexpr bool operator==(
			const fixed_string& a_lhs,
			const fixed_string& a_rhs) noexcept
		{
			return a_lhs.m_ref == a_rhs.m_ref;
		}

		// sucks for maps but I mainly want them sorted alphabetically
		[[nodiscard]] friend inline bool operator<(
			const fixed_string& a_lhs,
			const fixed_string& a_rhs) noexcept
		{
			return hash::stricmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
		}

		[[nodiscard]] friend inline bool operator>(
			const fixed_string& a_lhs,
			const fixed_string& a_rhs) noexcept
		{
			return hash::stricmp(a_lhs.c_str(), a_rhs.c_str()) > 0;
		}

		[[nodiscard]] friend inline bool operator>=(
			const fixed_string& a_lhs,
			const fixed_string& a_rhs) noexcept
		{
			return hash::stricmp(a_lhs.c_str(), a_rhs.c_str()) >= 0;
		}

		[[nodiscard]] friend inline bool operator<=(
			const fixed_string& a_lhs,
			const fixed_string& a_rhs) noexcept
		{
			return hash::stricmp(a_lhs.c_str(), a_rhs.c_str()) <= 0;
		}

		void set(const char* a_value);

		template <class Tv>
		void set_impl(Tv&& a_value);

		template <bool _Lock>
		__forceinline constexpr void try_release() noexcept
		{
			if (m_ref != string_cache::data_pool::get_empty())
			{
				string_cache::data_pool::try_release<_Lock>(m_ref);
			}
		}

		__forceinline void try_acquire() noexcept
		{
			if (m_ref != string_cache::data_pool::get_empty())
			{
				m_ref->inc_refcount();
			}
		}

		template <bool _Lock>
		__forceinline void assign_ref(const string_cache::icase_key<char>* a_ref) noexcept
		{
			if (a_ref != m_ref)
			{
				try_release<_Lock>();
				m_ref = a_ref;
				try_acquire();
			}
		}

		const string_cache::icase_key<char>* m_ref;

		template <class Archive>
		void save(Archive& ar, const unsigned int version) const
		{
			ar & m_ref->value();
		}

		template <class Archive>
		void load(Archive& ar, const unsigned int version)
		{
			std::string load;
			ar& load;

			set_impl(std::move(load));
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();
	};

	static_assert(sizeof(fixed_string) == 0x8);

	template <class Tv>
	void fixed_string::set_impl(Tv&& a_value)
	{
		using namespace string_cache;

		auto hash = icase_key<char>::compute_hash(a_value);

		IScopedLock lock(data_pool::get_lock());

		auto& value = data_pool::insert(hash, std::forward<Tv>(a_value));

		assign_ref<false>(std::addressof(value));
	}

	struct fixed_string_less_equal_p
	{
		[[nodiscard]] inline constexpr bool operator()(
			const stl::fixed_string& a_lhs,
			const stl::fixed_string& a_rhs) const noexcept
		{
			return a_lhs.less_equal_p(a_rhs);
		}
	};

}

namespace std
{
	template <>
	struct hash<stl::fixed_string>
	{
		[[nodiscard]] inline constexpr auto operator()(
			stl::fixed_string const& a_arg) const noexcept
		{
			return a_arg.hash();
		}
	};
}

#endif