#pragma once

#include <cstddef>
#include <xstring>
#include <utility>
#include <functional>
#include <cctype>
#include <cwctype>

#define ERRMSG_HASH_NOT_IMPL "Not implemented"

#include "STLCommon.h"
#include "PW.h"

namespace hash
{
    template <class T>
    inline constexpr bool is_int_wrapper_v =
        stl::is_any_base_of_v<std::remove_cv_t<T>,
        IntegralWrapper<bool>,
        IntegralWrapper<char>,
        IntegralWrapper<signed char>,
        IntegralWrapper<unsigned char>,
        IntegralWrapper<wchar_t>,
        IntegralWrapper<char16_t>,
        IntegralWrapper<char32_t>,
        IntegralWrapper<short>,
        IntegralWrapper<unsigned short>,
        IntegralWrapper<int>,
        IntegralWrapper<unsigned int>,
        IntegralWrapper<unsigned long>,
        IntegralWrapper<long long>,
        IntegralWrapper<unsigned long long>
        >;

    template <class T>
    using is_char_t = std::enable_if_t<
        std::is_same_v<char, T> ||
        std::is_same_v<unsigned char, T>, int>;

    template <class T>
    using is_wchar_t = std::enable_if_t<
        std::is_same_v<wchar_t, T> ||
        std::is_same_v<std::wint_t, T>, int>;

    template <class T, class U>
    using is_pair_int_char_t = std::enable_if_t<
        (std::is_integral_v<T> || is_int_wrapper_v<T>) && (
            std::is_same_v<std::pair<T, char>, std::pair<T, U>> ||
            std::is_same_v<std::pair<T, unsigned char>, std::pair<T, U>>), int>;

    template <class T, class U>
    using is_pair_int_wchar_t = std::enable_if_t<
        (std::is_integral_v<T> || is_int_wrapper_v<T>) && (
            std::is_same_v<std::pair<T, wchar_t>, std::pair<T, U>> ||
            std::is_same_v<std::pair<T, std::wint_t>, std::pair<T, U>>), int>;

    struct icase_comp
    {

        template <class T, class U, is_pair_int_char_t<T, U> = 0>
        bool operator()(std::pair<T, std::basic_string<U>> const& a_lhs, std::pair<T, std::basic_string<U>> const& a_rhs) const
        {
            if (a_lhs.first == a_rhs.first) {
                return _stricmp(a_lhs.second.c_str(), a_rhs.second.c_str()) < 0;
            }

            return a_lhs.first < a_rhs.first;
        }

        template <class T, class U, is_pair_int_wchar_t<T, U> = 0>
        bool operator()(std::pair<T, std::basic_string<U>> const& a_lhs, std::pair<T, std::basic_string<U>> const& a_rhs) const
        {
            if (a_lhs.first == a_rhs.first) {
                return _wcsicmp(a_lhs.second.c_str(), a_rhs.second.c_str()) < 0;
            }

            return a_lhs.first < a_rhs.first;
        }

        template <class T, is_char_t<T> = 0>
        bool operator() (const std::basic_string<T>& a_lhs, const std::basic_string<T>& a_rhs) const
        {
            return _stricmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
        }

        template <class T, is_wchar_t<T> = 0>
        bool operator() (const std::basic_string<T>& a_lhs, const std::basic_string<T>& a_rhs) const
        {
            return _wcsicmp(a_lhs.c_str(), a_rhs.c_str()) < 0;
        }

        template <class T>
        __declspec(noreturn) bool operator() (const T&, const T&) const
        {
            static_assert(false, ERRMSG_HASH_NOT_IMPL);
        }
    };

    struct iequal_to
    {
        template <class T, class U, is_pair_int_char_t<T, U> = 0>
        bool operator()(std::pair<T, std::basic_string<U>> const& a_lhs, std::pair<T, std::basic_string<U>> const& a_rhs) const
        {
            if (a_lhs.first != a_rhs.first ||
                a_lhs.second.size() != a_rhs.second.size())
            {
                return false;
            }

            return _stricmp(a_lhs.second.c_str(), a_rhs.second.c_str()) == 0;
        }

        template <class T, class U, is_pair_int_wchar_t<T, U> = 0>
        bool operator()(std::pair<T, std::basic_string<U>> const& a_lhs, std::pair<T, std::basic_string<U>> const& a_rhs) const
        {
            if (a_lhs.first != a_rhs.first ||
                a_lhs.second.size() != a_rhs.second.size())
            {
                return false;
            }

            return _wcsicmp(a_lhs.second.c_str(), a_rhs.second.c_str()) == 0;
        }

        template <class T, is_char_t<T> = 0>
        bool operator()(std::basic_string<T> const& a_lhs, std::basic_string<T> const& a_rhs) const
        {
            if (a_lhs.size() != a_rhs.size()) {
                return false;
            }

            return _stricmp(a_lhs.c_str(), a_rhs.c_str()) == 0;
        }

        template <class T, is_wchar_t<T> = 0>
        bool operator()(std::basic_string<T> const& a_lhs, std::basic_string<T> const& a_rhs) const
        {
            if (a_lhs.size() != a_rhs.size()) {
                return false;
            }

            return _wcsicmp(a_lhs.c_str(), a_rhs.c_str()) == 0;
        }

        template <class T>
        __declspec(noreturn) bool operator() (const T&, const T&) const
        {
            static_assert(false, ERRMSG_HASH_NOT_IMPL);
        }
    };

    /*__forceinline char toupper(char a_char)
    {
        return (a_char > 0x60 && a_char < 0x7B) ? a_char - 0x20 : a_char;
    }*/

    namespace fnv1
    {
        // 64 bit
        static inline constexpr std::size_t fnv_prime = 1099511628211ui64;
        static inline constexpr std::size_t fnv_offset_basis = 14695981039346656037ui64;

        __forceinline static std::size_t _append_hash_bytes_fnv1a(std::size_t a_hash, const std::uint8_t* const a_in, std::size_t a_size)
        {
            for (std::size_t i = 0; i < a_size; i++) {
                a_hash ^= static_cast<std::size_t>(a_in[i]);
                a_hash *= fnv_prime;
            }

            return a_hash;
        }

        __forceinline static std::size_t _append_hash_bytes_fnv1(std::size_t a_hash, const std::uint8_t* const a_in, std::size_t a_size)
        {
            for (std::size_t i = 0; i < a_size; i++) {
                a_hash *= fnv_prime;
                a_hash ^= static_cast<std::size_t>(a_in[i]);
            }

            return a_hash;
        }

        struct icase_fnv1
        {
            template <class T, class U, is_pair_int_char_t<T, U> = 0>
            std::size_t operator()(std::pair<T, std::basic_string<U>> const& a_in) const
            {
                auto p = reinterpret_cast<const std::uint8_t*>(std::addressof(a_in.first));

                std::size_t hash = _append_hash_bytes_fnv1(fnv_offset_basis, p, sizeof(T));

                for (auto e : a_in.second)
                {
                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>(std::toupper(static_cast<int>(e)));
                }

                return hash;
            }

            template <class T, class U, is_pair_int_wchar_t<T, U> = 0>
            std::size_t operator()(std::pair<T, std::basic_string<U>> const& a_in) const
            {
                auto p = reinterpret_cast<const std::uint8_t*>(std::addressof(a_in.first));

                std::size_t hash = _append_hash_bytes_fnv1(fnv_offset_basis, p, sizeof(T));

                for (auto e : a_in.second)
                {
                    auto c = std::towupper(static_cast<std::wint_t>(e));

                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>((c & 0xFF00) >> 8);
                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>(c & 0x00FF);
                }

                return hash;
            }

            template <class T, is_char_t<T> = 0>
            std::size_t operator()(std::basic_string<T> const& a_in) const
            {
                std::size_t hash = fnv_offset_basis;

                for (auto e : a_in)
                {
                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>(std::toupper(static_cast<int>(e)));
                }

                return hash;
            }

            template <class T, is_wchar_t<T> = 0>
            std::size_t operator()(std::basic_string<T> const& a_in) const
            {
                std::size_t hash = fnv_offset_basis;

                for (auto e : a_in)
                {
                    auto c = std::towupper(static_cast<std::wint_t>(e));

                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>((c & 0xFF00) >> 8);
                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>(c & 0x00FF);
                }

                return hash;
            }

            template <class T>
            void operator()(T const&) const
            {
                static_assert(false, ERRMSG_HASH_NOT_IMPL);
            }

        };

        struct icase_fnv1a
        {
            template <class T, class U, is_pair_int_char_t<T, U> = 0>
            std::size_t operator()(std::pair<T, std::basic_string<U>> const& a_in) const
            {
                auto p = reinterpret_cast<const std::uint8_t*>(std::addressof(a_in.first));

                std::size_t hash = _append_hash_bytes_fnv1a(fnv_offset_basis, p, sizeof(T));

                for (auto e : a_in.second)
                {
                    hash ^= static_cast<std::size_t>(std::toupper(static_cast<int>(e)));
                    hash *= fnv_prime;
                }

                return hash;
            }

            template <class T, class U, is_pair_int_wchar_t<T, U> = 0>
            std::size_t operator()(std::pair<T, std::basic_string<U>> const& a_in) const
            {
                auto p = reinterpret_cast<const std::uint8_t*>(std::addressof(a_in.first));

                std::size_t hash = _append_hash_bytes_fnv1a(fnv_offset_basis, p, sizeof(T));

                for (auto e : a_in.second)
                {
                    auto c = std::towupper(static_cast<std::wint_t>(e));

                    hash ^= static_cast<std::size_t>((c & 0xFF00) >> 8);
                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>(c & 0x00FF);
                    hash *= fnv_prime;

                }

                return hash;
            }

            template <class T, is_char_t<T> = 0>
            std::size_t operator()(std::basic_string<T> const& a_in) const
            {
                std::size_t hash = fnv_offset_basis;

                for (auto e : a_in)
                {
                    hash ^= static_cast<std::size_t>(std::toupper(static_cast<int>(e)));
                    hash *= fnv_prime;
                }

                return hash;
            }

            template <class T, is_wchar_t<T> = 0>
            std::size_t operator()(std::basic_string<T> const& a_in) const
            {
                std::size_t hash = fnv_offset_basis;

                for (auto e : a_in)
                {
                    auto c = std::towupper(static_cast<std::wint_t>(e));

                    hash ^= static_cast<std::size_t>((c & 0xFF00) >> 8);
                    hash *= fnv_prime;
                    hash ^= static_cast<std::size_t>(c & 0x00FF);
                    hash *= fnv_prime;
                }

                return hash;
            }

            template <class T>
            void operator()(T const&) const
            {
                static_assert(false, ERRMSG_HASH_NOT_IMPL);
            }

        };

        template <class T>
        std::size_t _compute_hash_fnv1a(T const& a_in)
        {
            static_assert(sizeof(T) > 0ui64);

            auto p = reinterpret_cast<const std::uint8_t*>(std::addressof(a_in));
            return _append_hash_bytes_fnv1a(fnv_offset_basis, p, sizeof(T));
        }

    }

    using i_fnv_1 = fnv1::icase_fnv1;
    using i_fnv_1a = fnv1::icase_fnv1a;
}

#define STD_SPECIALIZE_HASH(T) namespace std { template<> struct hash<T> { std::size_t operator()(T const& a_in) const noexcept { return ::hash::fnv1::_compute_hash_fnv1a(a_in); } };}
#define STD_SPECIALIZE_HASH_M(T, m) namespace std { template<> struct hash<T> { std::size_t operator()(T const& a_in) const noexcept { return ::hash::fnv1::_compute_hash_fnv1a(a_in ## . ## m); } };}
#define STD_SPECIALIZE_HASH_FWD(T,v) namespace std { template<> struct hash<T> { std::size_t operator()(T const& a_in) const noexcept { return hash<decltype(a_in ## . ## v)>()(a_in ## . ## v); } };}
