#pragma once

#include <vector>
#include <algorithm>

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#endif

namespace stl
{
    template <class K, class Pr = std::less_equal<K>>
    class set
    {
        //using comp_type = typename std::conditional_t<_Rev, std::greater<K>, std::less<K>>;

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
        friend class boost::serialization::access;
#endif

    public:

        using key_type = K;
        using value_type = K;

        using data_type = std::vector<value_type>;
        using iterator = typename data_type::iterator;
        using const_iterator = typename data_type::const_iterator;

        constexpr auto find(const key_type& a_key)
        {
            auto it = std::upper_bound(data.begin(), data.end(), a_key, Pr{});
            return it != data.end() && *it == a_key ? it : data.end();
        }

        constexpr auto find(const key_type& a_key) const
        {
            auto it = std::upper_bound(data.begin(), data.end(), a_key, Pr{});
            return it != data.end() && *it == a_key ? it : data.end();
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

        constexpr auto empty() const noexcept
        {
            return data.empty();
        }

        template <class... Args>
        constexpr std::pair<iterator, bool> emplace(Args&&... a_args)
        {
            auto n = key_type(std::forward<Args>(a_args)...);

            auto it = std::upper_bound(data.begin(), data.end(), n, Pr{});

            if (it != data.end() && *it == n)
            {
                return { it, false };
            }
            else
            {
                return { data.emplace(it, std::move(n)), true };
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
        
        constexpr auto size() const noexcept
        {
            return data.size();
        }

        /*constexpr auto& operator[](data_type::size_type a_index) noexcept
        {
            return data[a_index];
        }*/
        
        constexpr const auto& operator[](data_type::size_type a_index) const noexcept
        {
            return data[a_index];
        }

    protected:

        data_type data;

    private:

        /*inline bool comp(const_iterator a_it, const key_type& a_value) const
        {
            if (a_it != data.end() && *a_it == a_value)
            {
                return true;
            }

            if (a_it != data.begin())
            {
                return *std::prev(a_it) == a_value;
            }
            else
            {
                return false;
            }
        }*/

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
        template<class Archive>
        void serialize(Archive& ar, const unsigned int)
        {
            ar& data;
        }
#endif

    };
}