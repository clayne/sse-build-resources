#pragma once

namespace iterators
{
	template <class T>
	struct array_iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = T;
		using pointer = value_type*;
		using reference = value_type&;

		inline array_iterator(pointer a_ptr) :
			m_ptr(a_ptr)
		{
		}

		inline reference operator*() const
		{
			return *m_ptr;
		}

		inline pointer operator->()
		{
			return m_ptr;
		}

		inline array_iterator& operator++()
		{
			m_ptr++;
			return *this;
		}

		inline array_iterator operator++(int)
		{
			array_iterator tmp = *this;
			++(*this);
			return tmp;
		}

		inline friend constexpr bool operator==(const array_iterator& a, const array_iterator& b)
		{
			return a.m_ptr == b.m_ptr;
		};

		inline friend constexpr bool operator!=(const array_iterator& a, const array_iterator& b)
		{
			return a.m_ptr != b.m_ptr;
		};

	private:
		pointer m_ptr;
	};
}