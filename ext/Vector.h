#pragma once

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace stl
{
	template <class K, class V, class map_type = std::unordered_map<K, V>>
	class vectormap
	{
	public:

		using iterator = typename map_type::iterator;
		using const_iterator = typename map_type::const_iterator;

		using value_type = typename map_type::value_type;
		using mapped_type = typename map_type::mapped_type;
		using size_type = typename map_type::size_type;

		using value_pointer_type = value_type*;

		using vector_type = std::vector<value_pointer_type>;

		vectormap() = default;

		// vector maintains initializer list order
		vectormap(std::initializer_list<value_type> a_rhs)
		{
			for (auto& e : a_rhs)
			{
				emplace(e);
			}
		}

		vectormap& operator=(std::initializer_list<value_type> a_rhs)
		{
			clear();

			for (auto& e : a_rhs)
			{
				emplace(e);
			}

			return *this;
		}

		vectormap(const vectormap& a_rhs)
		{
			for (auto& e : a_rhs.m_vector)
			{
				emplace(*e);
			}
		}

		vectormap(vectormap&& a_rhs)
		{
			for (auto& e : a_rhs.m_vector)
			{
				emplace(e->first, std::move(e->second));
			}
		}

		vectormap& operator=(const vectormap& a_rhs)
		{
			clear();

			for (auto& e : a_rhs.m_vector)
			{
				emplace(*e);
			}

			return *this;
		}

		vectormap& operator=(vectormap&& a_rhs)
		{
			m_vector.clear();

			for (auto& e : a_rhs.m_vector)
			{
				emplace(e->first, std::move(e->second));
			}

			return *this;
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(K&& a_key, Args&&... a_args)
		{
			const auto r = m_map.try_emplace(std::move(a_key), std::forward<Args>(a_args)...);
			if (r.second)
				_insert_vec(std::addressof(*r.first));

			return r;
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(const K& a_key, Args&&... a_args)
		{
			const auto r = m_map.try_emplace(a_key, std::forward<Args>(a_args)...);
			if (r.second)
				_insert_vec(std::addressof(*r.first));

			return r;
		}

		template <class... Args>
		std::pair<iterator, bool> emplace(Args&&... a_args)
		{
			const auto r = m_map.emplace(std::forward<Args>(a_args)...);
			if (r.second)
				_insert_vec(std::addressof(*r.first));

			return r;
		}

		iterator erase(iterator a_it)
		{
			_erase_vec(std::addressof(*a_it));
			return m_map.erase(a_it);
		}

		const_iterator erase(const_iterator a_it)
		{
			_erase_vec(std::addressof(*a_it));
			return m_map.erase(a_it);
		}

		size_type erase(const K& a_key)
		{
			auto it = m_map.find(a_key);
			if (it == m_map.end())
				return size_type(0);

			_erase_vec(std::addressof(*it));

			return m_map.erase(it);
		}

		[[nodiscard]] SKMP_FORCEINLINE iterator find(const K& a_key)
		{
			return m_map.find(a_key);
		}

		[[nodiscard]] SKMP_FORCEINLINE const_iterator find(const K& a_key) const
		{
			return m_map.find(a_key);
		}

		[[nodiscard]] SKMP_FORCEINLINE iterator end() noexcept
		{
			return m_map.end();
		}

		[[nodiscard]] SKMP_FORCEINLINE iterator begin() noexcept
		{
			return m_map.begin();
		}

		[[nodiscard]] SKMP_FORCEINLINE const_iterator end() const noexcept
		{
			return m_map.end();
		}

		[[nodiscard]] SKMP_FORCEINLINE const_iterator begin() const noexcept
		{
			return m_map.begin();
		}

		[[nodiscard]] SKMP_FORCEINLINE const_iterator cend() const noexcept
		{
			return m_map.cend();
		}

		[[nodiscard]] SKMP_FORCEINLINE const_iterator cbegin() const noexcept
		{
			return m_map.cbegin();
		}

		[[nodiscard]] SKMP_FORCEINLINE void clear()
		{
			m_vector.clear();
			m_map.clear();
		}

		[[nodiscard]] SKMP_FORCEINLINE void release()
		{
			m_vector.swap(decltype(m_vector)());
			m_map.swap(decltype(m_map)());
		}

		[[nodiscard]] SKMP_FORCEINLINE map_type::size_type size() const noexcept
		{
			return m_map.size();
		}

		[[nodiscard]] SKMP_FORCEINLINE bool empty() const noexcept
		{
			return m_map.empty();
		}

		[[nodiscard]] SKMP_FORCEINLINE bool contains(const K& a_key) const
		{
			return m_map.contains(a_key);
		}

		[[nodiscard]] SKMP_FORCEINLINE constexpr auto getdata() noexcept
		{
			return m_vector.data();
		}

		[[nodiscard]] SKMP_FORCEINLINE constexpr const auto getdata() const noexcept
		{
			return m_vector.data();
		}

		[[nodiscard]] SKMP_FORCEINLINE constexpr const auto& getvec() const noexcept
		{
			return m_vector;
		}

		[[nodiscard]] SKMP_FORCEINLINE constexpr const auto& getmap() const noexcept
		{
			return m_map;
		}

		template <class Tp>
		void sortvec(Tp a_func)
		{
			std::sort(m_vector.begin(), m_vector.end(), a_func);
		}

	private:
		SKMP_FORCEINLINE void _insert_vec(value_pointer_type a_val)
		{
			m_vector.emplace_back(a_val);
		}

		SKMP_FORCEINLINE void _erase_vec(value_pointer_type a_val)
		{
			auto it = std::find(m_vector.begin(), m_vector.end(), a_val);
			//ASSERT(it != m_vector.end());
			m_vector.erase(it);
		}

		map_type m_map;
		vector_type m_vector;
	};

}