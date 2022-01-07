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
			for (auto& e : a_rhs.getvec())
			{
				emplace(*e);
			}
		}

		vectormap(vectormap&& a_rhs)
		{
			for (auto& e : a_rhs.getvec())
			{
				emplace(e->first, std::move(e->second));
			}
		}

		vectormap& operator=(const vectormap& a_rhs)
		{
			clear();

			for (auto& e : a_rhs.getvec())
			{
				emplace(*e);
			}

			return *this;
		}

		vectormap& operator=(vectormap&& a_rhs)
		{
			clear();

			for (auto& e : a_rhs.getvec())
			{
				emplace(e->first, std::move(e->second));
			}

			return *this;
		}

		template <class... Args>
		std::pair<iterator, bool> try_emplace(Args&&... a_args)
		{
			const auto r = m_map.try_emplace(std::forward<Args>(a_args)...);
			if (r.second)
			{
				_insert_vec(std::addressof(*r.first));
			}

			return r;
		}

		template <class... Args>
		std::pair<iterator, bool> emplace(Args&&... a_args)
		{
			const auto r = m_map.emplace(std::forward<Args>(a_args)...);
			if (r.second)
			{
				_insert_vec(std::addressof(*r.first));
			}

			return r;
		}

		auto erase(iterator a_it)
		{
			_erase_vec(std::addressof(*a_it));
			return m_map.erase(a_it);
		}

		auto erase(const_iterator a_it)
		{
			_erase_vec(std::addressof(*a_it));
			return m_map.erase(a_it);
		}

		auto erase(const K& a_key)
		{
			auto it = m_map.find(a_key);
			if (it == m_map.end())
			{
				return size_type(0);
			}

			_erase_vec(std::addressof(*it));

			return m_map.erase(it);
		}

		[[nodiscard]] inline auto find(const K& a_key)
		{
			return m_map.find(a_key);
		}

		[[nodiscard]] inline auto find(const K& a_key) const
		{
			return m_map.find(a_key);
		}

		[[nodiscard]] inline auto end() noexcept
		{
			return m_map.end();
		}

		[[nodiscard]] inline auto begin() noexcept
		{
			return m_map.begin();
		}

		[[nodiscard]] inline auto end() const noexcept
		{
			return m_map.end();
		}

		[[nodiscard]] inline auto begin() const noexcept
		{
			return m_map.begin();
		}

		[[nodiscard]] inline auto cend() const noexcept
		{
			return m_map.cend();
		}

		[[nodiscard]] inline auto cbegin() const noexcept
		{
			return m_map.cbegin();
		}

		void clear()
		{
			m_vector.clear();
			m_map.clear();
		}

		void release()
		{
			m_vector.swap(decltype(m_vector)());
			m_map.swap(decltype(m_map)());
		}

		[[nodiscard]] inline map_type::size_type size() const noexcept
		{
			return m_map.size();
		}

		[[nodiscard]] inline bool empty() const noexcept
		{
			return m_map.empty();
		}

		[[nodiscard]] inline bool contains(const K& a_key) const
		{
			return m_map.contains(a_key);
		}

		[[nodiscard]] inline constexpr auto getdata() noexcept
		{
			return m_vector.data();
		}

		[[nodiscard]] inline constexpr const auto getdata() const noexcept
		{
			return m_vector.data();
		}

		[[nodiscard]] inline constexpr const auto& getvec() const noexcept
		{
			return m_vector;
		}

		[[nodiscard]] inline constexpr const auto& getmap() const noexcept
		{
			return m_map;
		}

		template <class Tp>
		void sortvec(Tp a_func)
		{
			std::sort(m_vector.begin(), m_vector.end(), a_func);
		}

	private:
		inline void _insert_vec(value_pointer_type a_val)
		{
			m_vector.emplace_back(a_val);
		}

		inline void _erase_vec(value_pointer_type a_val)
		{
			auto it = std::find(m_vector.begin(), m_vector.end(), a_val);
			ASSERT(it != m_vector.end());
			m_vector.erase(it);
		}

		map_type m_map;
		vector_type m_vector;
	};

}