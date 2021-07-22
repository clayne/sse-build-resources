#if defined(SKMP_STRING_CACHE_INITIAL_SIZE)

#pragma warning(disable: 4073)
#pragma init_seg(lib)

#include "StringCache.h"

namespace string_cache
{
    data_storage data_storage::m_Instance;

    data_storage::data_storage()
    {
        m_data.reserve(SKMP_STRING_CACHE_INITIAL_SIZE);

        auto empty = std::string();

        auto hash = value_type::compute_hash(empty);

        auto& e = m_data.try_emplace(hash, std::move(empty)).first->second;

        m_icase_empty = std::addressof(e);
    }

    auto data_storage::insert(
        std::size_t hash,
        const std::string& a_string)
        ->
        const value_type&
    {
        return m_Instance.m_data.try_emplace(
            hash,
            hash,
            a_string).first->second;
    }

    auto data_storage::insert(
        std::size_t hash,
        std::string&& a_string)
        ->
        const value_type&
    {
        return m_Instance.m_data.try_emplace(
            hash,
            hash,
            std::move(a_string)).first->second;
    }

    auto data_storage::get_stats() noexcept
        ->
        stats_t
    {
        IScopedLock lock(get_lock());

        stats_t result;

        auto& data = m_Instance.m_data;

        result.count = data.size();

        /*for (stl::strip_type<decltype(data)>::size_type i = 0; i < data.bucket_count(); ++i) {
            std::size_t bucket_size = data.bucket_size(i);
            if (bucket_size == 0) {
                result.map_usage++;
            }
            else {
                result.map_usage += bucket_size;
            }
        }*/

        for (auto& entry : data)
        {
            result.key_data_usage += sizeof(stl::strip_type<decltype(entry)>);
            result.string_usage += entry.second.m_value.capacity() + sizeof(decltype(entry.second.m_value));

#if !defined(SKMP_STRING_CACHE_PERSIST)
            result.total_fixed_count += entry.second.use_count();
            result.total_fixed_size += entry.second.use_count() * sizeof(stl::fixed_string);

            result.estimated_uncached_usage += (entry.second.m_value.capacity() + sizeof(decltype(entry.second.m_value))) * entry.second.use_count();
#endif

        }

        result.total =
            result.map_usage +
            result.key_data_usage +
            result.string_usage
#if !defined(SKMP_STRING_CACHE_PERSIST)
            +result.total_fixed_size
#endif
            ;

        result.ratio =
            static_cast<long double>(result.total) /
            static_cast<long double>(result.estimated_uncached_usage);

        return result;
    }

    std::size_t icase_key::compute_hash(const std::string& a_value) noexcept
    {
        using namespace ::hash::fnv1;

        std::size_t hash = fnv_offset_basis;

        for (auto e : a_value)
        {
            hash ^= static_cast<std::size_t>(::hash::toupper(e));
            hash *= fnv_prime;
        }

        return hash;
    }
}

namespace stl
{
    fixed_string::fixed_string() :
        m_ref(string_cache::data_storage::get_empty())
    {
    }

    fixed_string::fixed_string(const std::string& a_value)
    {
        set(a_value);
    }

    fixed_string::fixed_string(std::string&& a_value)
    {
        set(std::move(a_value));
    }

    fixed_string::fixed_string(const char* a_value)
    {
        set(a_value);
    }

    fixed_string::fixed_string(const BSFixedString& a_value)
    {
        set(a_value.c_str());
    }

    fixed_string& fixed_string::operator=(const std::string& a_value)
    {
        set(a_value);
        return *this;
    }

    fixed_string& fixed_string::operator=(std::string&& a_value)
    {
        set(std::move(a_value));
        return *this;
    }

    fixed_string& fixed_string::operator=(const char* a_value)
    {
        set(a_value);
        return *this;
    }

    fixed_string& fixed_string::operator=(const BSFixedString& a_value)
    {
        set(a_value.c_str());
        return *this;
    }

    void fixed_string::set(const std::string& a_value)
    {
        using namespace string_cache;

        auto hash = icase_key::compute_hash(a_value);

        IScopedLock lock(data_storage::get_lock());

        auto& value = data_storage::insert(hash, a_value);

        copy_assign<false>(std::addressof(value));
    }

    void fixed_string::set(std::string&& a_value)
    {
        using namespace string_cache;

        auto hash = icase_key::compute_hash(a_value);

        IScopedLock lock(data_storage::get_lock());

        auto& value = data_storage::insert(hash, std::move(a_value));

        copy_assign<false>(std::addressof(value));
    }

    void fixed_string::set(const char* a_value)
    {
        if (a_value == nullptr)
        {
            m_ref = string_cache::data_storage::get_empty();
        }
        else
        {
            set(std::string(a_value));
        }
    }
}

#endif