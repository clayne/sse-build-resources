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

        auto& it = m_data.emplace(std::string()).first;

        m_icase_empty = std::addressof(*it);
    }

    auto data_storage::create(
        const std::string& a_string)
        ->
        const value_type&
    {
        return *m_Instance.m_data.emplace(
            a_string).first;
    }

    auto data_storage::create(
        std::string&& a_string)
        ->
        const value_type&
    {
        return *m_Instance.m_data.emplace(
            std::move(a_string)).first;
    }

    auto data_storage::create(
        std::size_t hash,
        const std::string& a_string)
        ->
        const value_type&
    {
        return *m_Instance.m_data.emplace(
            hash,
            a_string).first;
    }

    auto data_storage::create(
        std::size_t hash,
        std::string&& a_string)
        ->
        const value_type&
    {
        return *m_Instance.m_data.emplace(
            hash,
            std::move(a_string)).first;
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

            //sizeof(std::atomic<int32_t>)

            result.key_data_usage += sizeof(stl::strip_type<decltype(entry)>);
            result.string_usage += entry.m_value.capacity() + sizeof(decltype(entry.m_value));

#if !defined(SKMP_STRING_CACHE_PERSIST)
            result.total_fixed_count += entry.use_count();
            result.total_fixed_size += entry.use_count() * sizeof(stl::fixed_string);

            result.estimated_uncached_usage += (entry.m_value.capacity() + sizeof(decltype(entry.m_value))) * entry.use_count();
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

#if !defined(SKMP_STRING_CACHE_PERSIST)
    fixed_string::~fixed_string() noexcept
    {
        using namespace string_cache;

        if (auto r = m_ref.get(); r == nullptr || r == data_storage::get_empty()) {
            return;
        }

        IScopedLock lock(data_storage::get_lock());

        if (m_ref->use_count() == 1)
        {
            try
            {
                auto& key = static_cast<const icase_key&>(*m_ref);

                m_ref.reset();

                auto& data = data_storage::get_data();

                auto it = data.find(key);
                if (it != data.end())
                {
                    if (it->use_count() == 0)
                    { // no instance of fixed_string references this, delete it
                        data.erase(it);
                    }
                }
            }
            catch (const std::exception& e)
            {
                HALT(e.what());
            }
        }
        else
        {
            m_ref.reset();
        }
    }
#endif

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

        auto& value = data_storage::create(hash, a_value);

        m_ref = std::addressof(value);
    }

    void fixed_string::set(std::string&& a_value)
    {
        using namespace string_cache;

        auto hash = icase_key::compute_hash(a_value);

        IScopedLock lock(data_storage::get_lock());

        auto& value = data_storage::create(hash, std::move(a_value));

        m_ref = std::addressof(value);
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