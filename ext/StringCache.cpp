#if defined(SKMP_STRING_CACHE_INITIAL_SIZE)

#pragma warning(disable: 4073)
#pragma init_seg(lib)

#include "StringCache.h"
#include "Hash.h"

namespace string_cache
{
    data_pool data_pool::m_Instance;

    data_pool::data_pool()
    {
        m_data.reserve(SKMP_STRING_CACHE_INITIAL_SIZE);

        auto empty = std::string();

        auto& e = *m_data.emplace(value_type::compute_hash(empty), std::move(empty)).first;

        m_icase_empty = std::addressof(e);
    }

    auto data_pool::insert(
        std::size_t a_hash,
        const std::string& a_string)
        ->
        const value_type&
    {
        return *m_Instance.m_data.emplace(a_hash, a_string).first;
    }

    auto data_pool::insert(
        std::size_t a_hash,
        std::string&& a_string)
        ->
        const value_type&
    {
        return *m_Instance.m_data.emplace(a_hash, std::move(a_string)).first;
    }

#if !defined(SKMP_STRING_CACHE_PERSIST)
    void data_pool::release(
        const value_type* a_ref) noexcept
    {
        try
        {
            if (!get_data().erase(*a_ref)) {
                throw std::runtime_error("FIXME: orphaned value");
            }
        }
        catch (const std::exception& e)
        {
            HALT(e.what());
        }
    }

    template <bool _Lock>
    void data_pool::try_release(
        const value_type* a_ref) noexcept
    {
        if (!a_ref) {
            return;
        }

        if (a_ref == get_empty()) {
            return;
        }

        if (a_ref->dec_refcount() != 1) {
            return;
        }

        if constexpr (_Lock)
        {
            IScopedLock lock(get_lock());

            if (a_ref->use_count() == 0) {
                release(a_ref);
            }
        }
        else {
            release(a_ref);
        }
    }
#endif

    auto data_pool::get_stats() noexcept
        ->
        stats_t
    {
        IScopedLock lock(get_lock());

        stats_t result;

        auto& data = get_data();

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
            result.string_usage += entry.m_value.capacity() + sizeof(decltype(entry.m_value));

#if !defined(SKMP_STRING_CACHE_PERSIST)

            auto c = entry.use_count();

            result.total_fixed_count += c;
            result.total_fixed_size += c * sizeof(stl::fixed_string);

            result.estimated_uncached_usage += (entry.m_value.capacity() + sizeof(decltype(entry.m_value))) * c;
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
            hash ^= static_cast<std::size_t>(std::toupper(e));
            hash *= fnv_prime;
        }

        return hash;
    }
}

namespace stl
{
    using namespace string_cache;

    fixed_string::fixed_string() :
        m_ref(data_pool::get_empty())
    {
    }

#if !defined(SKMP_STRING_CACHE_PERSIST)

    fixed_string::~fixed_string()
    {
        data_pool::try_release<true>(m_ref);
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

    void fixed_string::set(const std::string& a_value)
    {
        auto hash = icase_key::compute_hash(a_value);

        IScopedLock lock(data_pool::get_lock());

        auto& value = data_pool::insert(hash, a_value);

#if !defined(SKMP_STRING_CACHE_PERSIST)
        copy_assign<false>(std::addressof(value));
#else
        m_ref = std::addressof(value);
#endif
    }

    void fixed_string::set(std::string&& a_value)
    {
        auto hash = icase_key::compute_hash(a_value);

        IScopedLock lock(data_pool::get_lock());

        auto& value = data_pool::insert(hash, std::move(a_value));

#if !defined(SKMP_STRING_CACHE_PERSIST)
        copy_assign<false>(std::addressof(value));
#else
        m_ref = std::addressof(value);
#endif
    }

    void fixed_string::set(const char* a_value)
    {
        if (a_value == nullptr)
        {
            m_ref = string_cache::data_pool::get_empty();
        }
        else
        {
            set(std::string(a_value));
        }
    }
}

#endif