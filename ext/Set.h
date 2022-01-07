#pragma once

#include "STLCommon.h"

#include <algorithm>
#include <vector>

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
#	include <boost/serialization/access.hpp>
#	include <boost/serialization/vector.hpp>
#endif

namespace stl
{
	template <class K, class... Args>
	struct in_place_key_extract_set
	{
		static constexpr bool extractable = false;
	};

	template <class K>
	struct in_place_key_extract_set<K, K>
	{
		static constexpr bool extractable = true;
		static const K& extract(const K& a_val) noexcept
		{
			return a_val;
		}
	};

	template <
		class K,
		class Pr = std::less_equal<K>,
		class Al = std::allocator<K>>
	class set
	{
#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
		friend class boost::serialization::access;
#endif

	public:
		using key_type = K;
		using value_type = K;
		using allocator = Al;

		using data_type = std::vector<value_type, allocator>;
		using const_iterator = typename data_type::const_iterator;
		using const_reverse_iterator = typename data_type::const_reverse_iterator;

		set() = default;

		set(std::initializer_list<value_type> a_init)
		{
			for (auto& e : a_init)
			{
				emplace_impl(e);
			}
		}

		set& operator=(std::initializer_list<value_type> a_init)
		{
			data.clear();
			for (auto& e : a_init)
			{
				emplace_impl(e);
			}
			return *this;
		}

		constexpr auto find(const key_type& a_key) const
		{
			auto it = find_upper_bound(a_key);
			return it != data.cend() && *it == a_key ? it : data.cend();
		}

		constexpr auto contains(const key_type& a_key) const
		{
			return !empty() ? find(a_key) != end() : false;
		}

		constexpr auto begin() const noexcept
		{
			return data.cbegin();
		}

		constexpr auto cbegin() const noexcept
		{
			return data.cbegin();
		}

		constexpr auto end() const noexcept
		{
			return data.end();
		}

		constexpr auto cend() const noexcept
		{
			return data.end();
		}

		constexpr auto rbegin() const noexcept
		{
			return data.rbegin();
		}

		constexpr auto rend() const noexcept
		{
			return data.rend();
		}

		constexpr auto crbegin() const noexcept
		{
			return data.crbegin();
		}

		constexpr auto crend() const noexcept
		{
			return data.crend();
		}

		inline constexpr auto size() const noexcept
		{
			return data.size();
		}

		inline constexpr auto empty() const noexcept
		{
			return data.empty();
		}

		template <class... Args>
		constexpr std::pair<const_iterator, bool> emplace(Args&&... a_args)
		{
			return emplace_impl(std::forward<Args>(a_args)...);
		}

		constexpr std::pair<const_iterator, bool> insert(const value_type& a_value)
		{
			return emplace_impl(a_value);
		}

		constexpr std::pair<const_iterator, bool> insert(value_type&& a_value)
		{
			return emplace_impl(std::move(a_value));
		}

		constexpr void insert(std::initializer_list<value_type> a_init)
		{
			for (auto& e : a_init)
			{
				emplace_impl(e);
			}
		}

		constexpr auto erase(const key_type& a_key)
		{
			auto it = find(a_key);
			return it != end() ? data.erase(it) : it;
		}

		constexpr auto erase(const_iterator a_it)
		{
			return data.erase(a_it);
		}

		constexpr void reserve(data_type::size_type a_capacity)
		{
			data.reserve(a_capacity);
		}

		constexpr void shrink_to_fit()
		{
			data.shrink_to_fit();
		}

		constexpr void clear() noexcept
		{
			data.clear();
		}

	private:
		template <class... Args>
		std::pair<const_iterator, bool> emplace_impl(Args&&... a_args)
		{
			using in_place_key_extractor = typename in_place_key_extract_set<key_type, std::remove_cvref_t<Args>...>;

			if constexpr (in_place_key_extractor::extractable)
			{
				const auto& k = in_place_key_extractor::extract(a_args...);

				auto it = find_upper_bound(k);

				if (it != data.cend() && *it == k)
				{
					return { it, false };
				}
				else
				{
					return { data.emplace(it, std::forward<Args>(a_args)...), true };
				}
			}
			else
			{
				key_type k(std::forward<Args>(a_args)...);

				auto it = find_upper_bound(k);

				if (it != data.cend() && *it == k)
				{
					return { it, false };
				}
				else
				{
					return { data.emplace(it, std::move(k)), true };
				}
			}
		}

		inline constexpr auto find_upper_bound(const key_type& a_key) const
		{
			return std::upper_bound(data.begin(), data.end(), a_key, Pr{});
		}

		constexpr void sort_data()
		{
			std::sort(data.begin(), data.end(), Pr{});
		}

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
		template <class Archive>
		void save(Archive& ar, const unsigned int) const
		{
			ar& data;
		}

		template <class Archive>
		void load(Archive& ar, const unsigned int)
		{
			data_type tmp;
			ar& tmp;

			data.clear();

			for (auto& e : tmp)
			{
				emplace_impl(std::move(e));
			}

			data.shrink_to_fit();
		}

		BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

		data_type data;
	};
}