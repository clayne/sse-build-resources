#pragma once

//#define SKMP_STRING_CACHE_PERSIST

#if defined(SKMP_STRING_CACHE_INITIAL_SIZE)

#include <boost/serialization/string.hpp> 
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/version.hpp>

#include <json/json.h>

#include <string>
#include <memory>
#include <unordered_set>

//#include <tsl/sparse_set.h>

#include "STLCommon.h"

namespace stl
{
    class fixed_string;
};

namespace string_cache
{
    class key_ref_wrapper;

    class __declspec(novtable) key_base
    {
        friend class key_ref_wrapper;

    public:

        key_base(
            std::size_t a_hash,
            const std::string& a_value)
            :
            m_hash(a_hash),
            m_value(a_value)
        {
        }

        key_base(
            std::size_t a_hash,
            std::string&& a_value)
            :
            m_hash(a_hash),
            m_value(std::move(a_value))
        {
        }

        key_base(const key_base&) = delete;
        key_base(key_base&&) = delete;
        key_base& operator=(const key_base&) = delete;
        key_base& operator=(key_base&&) = delete;

        [[nodiscard]] inline auto hash() const noexcept {
            return m_hash;
        }

        [[nodiscard]] inline const auto& value() const noexcept {
            return m_value;
        }

#if !defined(SKMP_STRING_CACHE_PERSIST)

        [[nodiscard]] inline auto use_count() const noexcept {
            return m_refcount.load(std::memory_order_acquire);
        }

#endif

        friend inline bool operator==(
            const key_base& a_lhs,
            const key_base& a_rhs)
        {
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
    private:

        inline long inc_refcount() const noexcept {
            return m_refcount.fetch_add(1, std::memory_order_relaxed);
        }

        inline long dec_refcount() const noexcept {
            return m_refcount.fetch_sub(1, std::memory_order_acq_rel);
        }

        mutable std::atomic_long m_refcount{ 0l };
#endif

    };

    class icase_key :
        public key_base
    {
    public:

        icase_key(
            const std::string& a_value) noexcept
            :
            key_base(
                compute_hash(a_value),
                a_value
            )
        {
        }

        icase_key(
            std::string&& a_value) noexcept
            :
            key_base(
                compute_hash(a_value),
                std::move(a_value)
            )
        {
        }

        using key_base::key_base;

        static std::size_t compute_hash(const std::string& a_value) noexcept;

    };


#if !defined(SKMP_STRING_CACHE_PERSIST)
    class key_ref_wrapper
    {
        friend class stl::fixed_string;

    public:

        key_ref_wrapper()
            :
            m_ref(nullptr)
        {
        }

        ~key_ref_wrapper() noexcept
        {
            reset();
        }

        explicit key_ref_wrapper(const key_base* a_ref) noexcept
            :
            m_ref(a_ref)
        {
            if (a_ref) {
                a_ref->inc_refcount();
            }
        }

        key_ref_wrapper& operator=(const key_base* a_ref) noexcept
        {
            _copy_assign(a_ref);
            return *this;
        }

        explicit key_ref_wrapper(const key_ref_wrapper& a_rhs) noexcept
            :
            m_ref(a_rhs.m_ref)
        {
            if (m_ref) {
                m_ref->inc_refcount();
            }
        }

        explicit key_ref_wrapper(key_ref_wrapper&& a_rhs) noexcept
            :
            m_ref(a_rhs.m_ref)
        {
            if (m_ref) {
                m_ref->inc_refcount();
            }
        }

        key_ref_wrapper& operator=(const key_ref_wrapper& a_rhs) noexcept
        {
            _copy_assign(a_rhs.m_ref);
            return *this;
        }

        key_ref_wrapper& operator=(key_ref_wrapper&& a_rhs) noexcept
        {
            _copy_assign(a_rhs.m_ref);
            return *this;
        }

        inline auto operator->() const noexcept {
            return m_ref;
        }

        inline auto& operator*() const noexcept {
            return *m_ref;
        }

        inline auto get() const noexcept
        {
            return m_ref;
        }

    private:

        const key_base* m_ref;

        inline void reset() noexcept
        {
            if (m_ref) {
                m_ref->dec_refcount();
                m_ref = nullptr;
            }
        }

        inline void _copy_assign(const key_base* a_ref) noexcept
        {
            auto c = m_ref;

            m_ref = a_ref;

            if (a_ref) {
                a_ref->inc_refcount();
            }

            if (c) {
                c->dec_refcount();
            }
        }
    };
#endif
}

namespace std
{
    template<>
    struct hash<string_cache::icase_key>
    {
        auto operator()(string_cache::icase_key const& arg) const
        {
            return arg.hash();
        }
    };
}

namespace string_cache
{
    class data_storage
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
        data_storage();

        [[nodiscard]] inline static auto size() noexcept {
            return m_Instance.m_data.size();
        }

        [[nodiscard]] static stats_t get_stats() noexcept;

        /*SKMP_FORCEINLINE static auto& get_rqueue() {
            return m_Instance.m_rqueue;
        }*/

    private:

        static const value_type& create(
            const std::string& a_string);

        static const value_type& create(
            std::string&& a_string);

        static const value_type& create(
            std::size_t hash,
            const std::string& a_string);

        static const value_type& create(
            std::size_t hash,
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

        FastSpinMutex m_lock;

        const icase_key* m_icase_empty;

        // according to spec references/pointers are only invalidated when deleted
        std::unordered_set<value_type> m_data;

        static data_storage m_Instance;

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
        ~fixed_string() noexcept;
#endif

        fixed_string(const std::string& a_value);
        fixed_string(std::string&& a_value);
        fixed_string(const char* a_value);
        fixed_string(const BSFixedString& a_value);

        fixed_string& operator=(const std::string& a_value);
        fixed_string& operator=(std::string&& a_value);
        fixed_string& operator=(const char* a_value);
        fixed_string& operator=(const BSFixedString& a_value);

        [[nodiscard]] inline operator const std::string& () const noexcept {
            return m_ref->m_value;
        }

        [[nodiscard]] inline operator Json::Value() const noexcept {
            return m_ref->m_value;
        }

        [[nodiscard]] inline auto hash() const noexcept {
            return m_ref->m_hash;
        }

        [[nodiscard]] inline const auto& get() const noexcept {
            return m_ref->m_value;
        }

        [[nodiscard]] inline auto data() const noexcept {
            return m_ref->m_value.data();
        }

        [[nodiscard]] inline auto c_str() const noexcept {
            return m_ref->m_value.c_str();
        }

        [[nodiscard]] inline auto size() const noexcept {
            return m_ref->m_value.size();
        }

        [[nodiscard]] inline auto empty() const noexcept {
            return m_ref->m_value.empty();
        }

        [[nodiscard]] inline auto ref() const noexcept {
            return
#if !defined(SKMP_STRING_CACHE_PERSIST)
                m_ref.get();
#else
                m_ref;
#endif
        }

        [[nodiscard]] friend inline bool operator==(
            const fixed_string& a_lhs,
            const fixed_string& a_rhs)
        {
            return a_lhs.ref() == a_rhs.ref();
        }

        // sucks for maps but I mainly want them sorted alphabetically
        [[nodiscard]] friend inline bool operator<(
            const fixed_string& a_lhs,
            const fixed_string& a_rhs)
        {
            return _stricmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
        }

    private:

#if !defined(SKMP_STRING_CACHE_PERSIST)
        string_cache::key_ref_wrapper m_ref;
#else
        const string_cache::key_base* m_ref;
#endif

        void set(const std::string& a_value);
        void set(std::string&& a_value);
        void set(const char* a_value);

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

}

namespace std
{
    template<>
    struct hash<stl::fixed_string>
    {
        auto operator()(stl::fixed_string const& arg) const {
            return arg.hash();
        }
    };
}

#endif