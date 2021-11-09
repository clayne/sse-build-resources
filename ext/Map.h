#pragma once

#include <vector>
#include <algorithm>

#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#endif

namespace stl
{
    template <class K, class V>
    class map
    {
#if !defined(_SKMP_DISABLE_BOOST_SERIALIZATION)
        friend class boost::serialization::access;
#endif

    public:

        using key_type = K;
        using value_type = std::pair<K, V>;
        using mapped_type = V;

        using data_type = std::vector<value_type>;
        using iterator = typename data_type::iterator;
        using const_iterator = typename data_type::const_iterator;

        struct comp_pred
        {
            [[nodiscard]] inline constexpr bool operator()(
                const key_type& a_key, 
                const value_type& a_value) const noexcept
            {
                return a_value.first >= a_key;
            }
        };

        constexpr auto find(const key_type& a_key)
        {
            auto it = std::upper_bound(data.begin(), data.end(), a_key, comp_pred{});
            return it != data.end() && it->first == a_key ? it : data.end();
        }

        constexpr auto find(const key_type& a_key) const
        {
            auto it = std::upper_bound(data.begin(), data.end(), a_key, comp_pred{});
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

        constexpr auto empty() const noexcept
        {
            return data.empty();
        }

        template <class... Args>
        constexpr std::pair<iterator, bool> emplace(const key_type& a_key, Args&&... a_args)
        {
            auto it = std::upper_bound(data.begin(), data.end(), a_key, comp_pred{});

            if (it != data.end() && it->first == a_key)
            {
                return { it, false };
            }
            else
            {
                return { data.emplace(it, std::make_pair(a_key, mapped_type{std::forward<Args>(a_args)...})), true };
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