#pragma once

//#define SKMP_STRING_CACHE_PERSIST

#if defined(SKMP_STRING_CACHE_INITIAL_SIZE)

#include <boost/serialization/string.hpp> 
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>

#include <string>
#include <memory>
#include <unordered_set>

#include "STLCommon.h"
#include "Threads.h"

namespace string_cache
{
    class data_pool;

    class icase_key
    {
        friend class data_pool;

    public:

        icase_key() = delete;

        icase_key(
            std::size_t a_hash,
            const std::string& a_value)
            :
            m_hash(a_hash),
            m_value(a_value)
        {
        }

        icase_key(
            std::size_t a_hash,
            std::string&& a_value)
            :
            m_hash(a_hash),
            m_value(std::move(a_value))
        {
        }

        icase_key(const icase_key&) = delete;
        icase_key(icase_key&&) = delete;
        icase_key& operator=(const icase_key&) = delete;
        icase_key& operator=(icase_key&&) = delete;

        [[nodiscard]] inline constexpr auto hash() const noexcept {
            return m_hash;
        }

        [[nodiscard]] inline constexpr const auto& value() const noexcept {
            return m_value;
        }

        [[nodiscard]] friend inline bool operator==(
            const icase_key& a_lhs,
            const icase_key& a_rhs) noexcept
        {
            if (a_lhs.hash() != a_rhs.hash()) {
                return false;
            }

            auto& lhs_data = a_lhs.value();
            auto& rhs_data = a_rhs.value();

            if (lhs_data.size() != rhs_data.size()) {
                return false;
            }

            return _stricmp(lhs_data.c_str(), rhs_data.c_str()) == 0;
        }

        const std::size_t m_hash;
        const std::string m_value;

#if !defined(SKMP_STRING_CACHE_PERSIST)

        static std::size_t compute_hash(const std::string& a_value) noexcept;

        [[nodiscard]] inline auto use_count() const noexcept {
            return m_refcount.load(std::memory_order_acquire);
        }

        inline void inc_refcount() const noexcept {
            m_refcount.fetch_add(1, std::memory_order_relaxed);
        }

        [[nodiscard]] inline auto dec_refcount() const noexcept {
            return m_refcount.fetch_sub(1, std::memory_order_acq_rel);
        }

    private:

        mutable std::atomic<std::uint64_t> m_refcount{ 0 };

        static_assert(decltype(m_refcount)::is_always_lock_free);
#endif

    };

}

namespace std
{
    template<>
    struct hash<string_cache::icase_key>
    {
        [[nodiscard]] inline auto operator()(string_cache::icase_key const& a_arg) const noexcept
        {
            return a_arg.hash();
        }
    };
}

namespace stl
{
    class fixed_string;
};

namespace string_cache
{
    class data_pool
    {
        friend class stl::fixed_string;

        using value_type = icase_key;

        struct stats_t
        {
            std::size_t count{ 0 };
            std::size_t map_usage{ 0 };
            std::size_t key_data_usage{ 0 };
            std::size_t string_usage{ 0 };

#if !defined(SKMP_STRING_CACHE_PERSIST)
            std::size_t total_fixed_count{ 0 };
            std::size_t total_fixed_size{ 0 };

            std::size_t estimated_uncached_usage{ 0 };
#endif

            std::size_t total{ 0 };
            long double ratio{ 0 };
        };

    public:
        data_pool();

        [[nodiscard]] inline static auto size() noexcept {
            return m_Instance.m_data.size();
        }

        [[nodiscard]] static stats_t get_stats() noexcept;

        /*inline static auto& get_rqueue() {
            return m_Instance.m_rqueue;
        }*/

    private:

        static const value_type& insert(
            std::size_t a_hash,
            const std::string& a_string);

        static const value_type& insert(
            std::size_t a_hash,
            std::string&& a_string);

        [[nodiscard]] inline static auto& get_lock() noexcept {
            return m_Instance.m_lock;
        }

        [[nodiscard]] inline static auto get_empty() noexcept {
            return m_Instance.m_icase_empty;
        }

        [[nodiscard]] inline static auto& get_data() noexcept {
            return m_Instance.m_data;
        }


#if !defined(SKMP_STRING_CACHE_PERSIST)

        static void release(
            const value_type* a_ref) noexcept;

        template <bool _Lock>
        static void try_release(
            const value_type* a_ref) noexcept;

#endif

        FastSpinLock m_lock;

        const icase_key* m_icase_empty;

        std::unordered_set<value_type> m_data;

        static data_pool m_Instance;
    };
}

namespace stl
{

    // case insensitive
    class fixed_string
    {
        friend class boost::serialization::access;

    public:

        fixed_string();

#if !defined(SKMP_STRING_CACHE_PERSIST)
        ~fixed_string();

        fixed_string(const fixed_string& a_rhs)
            :
            m_ref(a_rhs.m_ref)
        {
            acquire(m_ref);
        }

        fixed_string(fixed_string&& a_rhs)
            :
            m_ref(a_rhs.m_ref)
        {
            a_rhs.m_ref = nullptr;
        }

        fixed_string& operator=(const fixed_string& a_rhs)
        {
            if (this != std::addressof(a_rhs)) {
                copy_assign<true>(a_rhs.m_ref);
            }
            return *this;
        }

        fixed_string& operator=(fixed_string&& a_rhs)
        {
            if (this != std::addressof(a_rhs))
            {
                auto old = m_ref;

                m_ref = a_rhs.m_ref;
                a_rhs.m_ref = nullptr;

                string_cache::data_pool::try_release<true>(old);
            }
            return *this;
        }
#endif

        fixed_string(const std::string& a_value);
        fixed_string(std::string&& a_value);
        fixed_string(const char* a_value);

        fixed_string& operator=(const std::string& a_value);
        fixed_string& operator=(std::string&& a_value);
        fixed_string& operator=(const char* a_value);

        [[nodiscard]] inline constexpr operator const auto& () const noexcept {
            return m_ref->m_value;
        }

        [[nodiscard]] inline constexpr auto hash() const noexcept {
            return m_ref->m_hash;
        }

        [[nodiscard]] inline constexpr const auto& get() const noexcept {
            return m_ref->m_value;
        }

        [[nodiscard]] inline constexpr auto data() const noexcept {
            return m_ref->m_value.data();
        }

        [[nodiscard]] inline constexpr auto c_str() const noexcept {
            return m_ref->m_value.c_str();
        }

        [[nodiscard]] inline constexpr auto size() const noexcept {
            return m_ref->m_value.size();
        }

        [[nodiscard]] inline constexpr auto empty() const noexcept {
            return m_ref->m_value.empty();
        }

    private:

        [[nodiscard]] inline constexpr auto ref() const noexcept {
            return m_ref;
        }

        [[nodiscard]] friend inline constexpr bool operator==(
            const fixed_string& a_lhs,
            const fixed_string& a_rhs) noexcept
        {
            return a_lhs.ref() == a_rhs.ref();
        }

        // sucks for maps but I mainly want them sorted alphabetically
        [[nodiscard]] friend inline bool operator<(
            const fixed_string& a_lhs,
            const fixed_string& a_rhs) noexcept
        {
            return _stricmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
        }

        void set(const std::string& a_value);
        void set(std::string&& a_value);
        void set(const char* a_value);

#if !defined(SKMP_STRING_CACHE_PERSIST)
        template <bool _Lock>
        inline void copy_assign(const string_cache::icase_key* a_ref) noexcept
        {
            auto old = m_ref;

            m_ref = a_ref;
            acquire(a_ref);

            string_cache::data_pool::try_release<_Lock>(old);
        }

        static inline void acquire(const string_cache::icase_key* a_ref) noexcept
        {
            if (a_ref && a_ref != string_cache::data_pool::get_empty()) {
                a_ref->inc_refcount();
            }
        }
#endif

        const string_cache::icase_key* m_ref{ nullptr };

    private:

        template<class Archive>
        void save(Archive& ar, const unsigned int version) const
        {
            ar& m_ref->m_value;
        }

        template<class Archive>
        void load(Archive& ar, const unsigned int version)
        {
            std::string load;
            ar& load;

            set(std::move(load));
        }

        BOOST_SERIALIZATION_SPLIT_MEMBER()
    };

    static_assert(sizeof(fixed_string) == 0x8);
}

namespace std
{
    template<>
    struct hash<stl::fixed_string>
    {
        [[nodiscard]] inline auto operator()(stl::fixed_string const& a_arg) const noexcept {
            return a_arg.hash();
        }
    };
}

#endif