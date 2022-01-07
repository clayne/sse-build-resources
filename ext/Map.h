#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
#	include <boost/serialization/access.hpp>
#	include <boost/serialization/vector.hpp>
#endif

namespace stl
{
	template <class K, class... Args>
	struct in_place_key_extract_map
	{
		static constexpr bool extractable = false;
	};

	template <class K, class S>
	struct in_place_key_extract_map<K, K, S>
	{
		static constexpr bool extractable = true;
		static const K& extract(const K& a_val, const S&) noexcept
		{
			return a_val;
		}
	};

	template <class K, class F, class S>
	struct in_place_key_extract_map<K, std::pair<F, S>>
	{
		static constexpr bool extractable = std::is_same_v<K, std::remove_cvref_t<F>>;
		static const K& extract(const std::pair<F, S>& a_val)
		{
			return a_val.first;
		}
	};

	template <
		class K,
		class V,
		class Pr = std::less_equal<K>,
		class Al = std::allocator<std::pair<K, V>>>
	class map
	{
#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
		friend class boost::serialization::access;
#endif

	public:
		using key_type = K;
		using mapped_type = V;
		using value_type = std::pair<K, V>;

		using allocator = Al;

		using data_type = std::vector<value_type, allocator>;
		using iterator = typename data_type::iterator;
		using const_iterator = typename data_type::const_iterator;
		using const_reverse_iterator = typename data_type::const_reverse_iterator;

	private:
		struct comp_wrapper
		{
			[[nodiscard]] inline constexpr bool operator()(
				const key_type& a_key,
				const value_type& a_value) const noexcept
			{
				return Pr{}(a_key, a_value.first);
			}
		};

		struct sort_comp_wrapper
		{
			[[nodiscard]] inline constexpr bool operator()(
				const value_type& a_lhs,
				const value_type& a_rhs) const noexcept
			{
				return Pr{}(a_lhs.first, a_rhs.first);
			}
		};

	public:
		map() = default;

		map(std::initializer_list<value_type> a_init)
		{
			for (auto& e : a_init)
			{
				emplace_impl(e);
			}
		}

		map& operator=(std::initializer_list<value_type> a_init)
		{
			data.clear();
			for (auto& e : a_init)
			{
				emplace_impl(e);
			}
			return *this;
		}

		constexpr auto find(const key_type& a_key)
		{
			auto it = find_upper_bound(a_key);
			return it != data.end() && it->first == a_key ? it : data.end();
		}

		constexpr auto find(const key_type& a_key) const
		{
			auto it = find_upper_bound(a_key);
			return it != data.end() && it->first == a_key ? it : data.end();
		}

		constexpr auto contains(const key_type& a_key) const
		{
			return !empty() ? find(a_key) != data.end() : false;
		}

		constexpr auto begin() const noexcept
		{
			return data.cbegin();
		}

		constexpr auto begin() noexcept
		{
			return data.begin();
		}

		constexpr auto end() const noexcept
		{
			return data.cend();
		}

		constexpr auto end() noexcept
		{
			return data.end();
		}

		constexpr auto rbegin() noexcept
		{
			return data.rbegin();
		}

		constexpr auto rend() noexcept
		{
			return data.rend();
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

		constexpr auto empty() const noexcept
		{
			return data.empty();
		}

		template <class... Args>
		constexpr std::pair<iterator, bool> try_emplace(const key_type& a_key, Args&&... a_args)
		{
			return try_emplace_impl(a_key, std::forward<Args>(a_args)...);
		}

		template <class... Args>
		constexpr std::pair<iterator, bool> try_emplace(key_type&& a_key, Args&&... a_args)
		{
			return try_emplace_impl(std::move(a_key), std::forward<Args>(a_args)...);
		}

		template <class... Args>
		constexpr std::pair<iterator, bool> emplace(Args&&... a_args)
		{
			return emplace_impl(std::forward<Args>(a_args)...);
		}

		constexpr std::pair<iterator, bool> insert(const value_type& a_value)
		{
			return emplace_impl(a_value);
		}

		constexpr std::pair<iterator, bool> insert(value_type&& a_value)
		{
			return emplace_impl(std::move(a_value));
		}

		void insert(std::initializer_list<value_type> a_init)
		{
			for (auto& e : a_init)
			{
				emplace_impl(e);
			}
		}

		constexpr std::pair<iterator, bool> insert_or_assign(const key_type& a_key, mapped_type&& a_value)
		{
			return insert_or_assign_impl(a_key, std::forward<mapped_type>(a_value));
		}

		constexpr std::pair<iterator, bool> insert_or_assign(key_type&& a_key, mapped_type&& a_value)
		{
			return insert_or_assign_impl(std::move(a_key), std::forward<mapped_type>(a_value));
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

		constexpr auto size() const noexcept
		{
			return data.size();
		}

		constexpr mapped_type& operator[](const key_type& a_key)
		{
			return try_emplace(a_key).first->second;
		}

		[[nodiscard]] constexpr mapped_type& at(const key_type& a_key)
		{
			auto it = find(a_key);
			if (it == end())
			{
				throw std::out_of_range("invalid map key");
			}
			return it->second;
		}

		[[nodiscard]] constexpr const mapped_type& at(const key_type& a_key) const
		{
			auto it = find(a_key);
			if (it == end())
			{
				throw std::out_of_range("invalid map key");
			}
			return it->second;
		}

	protected:
		data_type data;

	private:
		template <class Tk, class... Args>
		constexpr std::pair<iterator, bool> try_emplace_impl(Tk&& a_key, Args&&... a_args)
		{
			auto it = find_upper_bound(a_key);

			if (it != data.end() && it->first == a_key)
			{
				return { it, false };
			}
			else
			{
				return { data.emplace(
							 it,
							 std::piecewise_construct,
							 std::forward_as_tuple(std::forward<Tk>(a_key)),
							 std::forward_as_tuple(std::forward<Args>(a_args)...)),
					     true };
			}
		}

		template <class... Args>
		std::pair<iterator, bool> emplace_impl(Args&&... a_args)
		{
			using in_place_key_extractor = typename in_place_key_extract_map<key_type, std::remove_cvref_t<Args>...>;

			if constexpr (in_place_key_extractor::extractable)
			{
				const auto& k = in_place_key_extractor::extract(a_args...);

				auto it = find_upper_bound(k);

				if (it != data.end() && it->first == k)
				{
					return { it, false };
				}
				else
				{
					return { data.emplace(
								 it,
								 std::forward<Args>(a_args)...),
						     true };
				}
			}
			else
			{
				value_type k(std::forward<Args>(a_args)...);

				auto it = find_upper_bound(k.first);

				if (it != data.end() && it->first == k.first)
				{
					return { it, false };
				}
				else
				{
					return { data.emplace(it, std::move(k)), true };
				}
			}
		}

		template <class Tk, class Tm>
		constexpr std::pair<iterator, bool> insert_or_assign_impl(Tk&& a_key, Tm&& a_value)
		{
			auto it = find_upper_bound(a_key);

			if (it != data.end() && it->first == a_key)
			{
				it->second = std::forward<Tm>(a_value);

				return { it, false };
			}
			else
			{
				return { data.emplace(
							 it,
							 std::forward<Tk>(a_key),
							 std::forward<Tm>(a_value)),
					     true };
			}
		}

		inline constexpr auto find_upper_bound(const key_type& a_key) const
		{
			return std::upper_bound(data.begin(), data.end(), a_key, comp_wrapper{});
		}

		inline constexpr auto find_upper_bound(const key_type& a_key)
		{
			return std::upper_bound(data.begin(), data.end(), a_key, comp_wrapper{});
		}

		constexpr void sort_data()
		{
			std::sort(data.begin(), data.end(), sort_comp_wrapper{});
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
	};
}