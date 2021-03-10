#pragma once

#include <type_traits>

namespace Enum
{
    template <typename T>
    constexpr typename std::underlying_type<T>::type Underlying(T e) noexcept {
        return static_cast<typename std::underlying_type<T>::type>(e);
    }

    template <typename T>
    constexpr typename std::underlying_type<T>::type* Underlying(T* e) noexcept {
        return reinterpret_cast<typename std::underlying_type<T>::type*>(e);
    }

    template <class T>
    using is_scoped = std::integral_constant<bool, !std::is_convertible_v<T, int>&& std::is_enum_v<T>>;
}

#define DEFINE_ENUM_CLASS_BITWISE(x) \
    SKMP_FORCEINLINE constexpr x operator|(x a_lhs, x a_rhs) noexcept { \
        return static_cast<x>(Enum::Underlying(a_lhs) | Enum::Underlying(a_rhs)); } \
    SKMP_FORCEINLINE constexpr x operator&(x a_lhs, x a_rhs) noexcept { \
        return static_cast<x>(Enum::Underlying(a_lhs) & Enum::Underlying(a_rhs)); } \
    SKMP_FORCEINLINE constexpr x operator^(x a_lhs, x a_rhs) noexcept { \
        return static_cast<x>(Enum::Underlying(a_lhs) ^ Enum::Underlying(a_rhs)); } \
    SKMP_FORCEINLINE constexpr x operator~(x a_lhs) noexcept { \
        return static_cast<x>(~Enum::Underlying(a_lhs)); } \
    SKMP_FORCEINLINE constexpr x& operator|=(x &a_lhs, x a_rhs) noexcept { \
        a_lhs = static_cast<x>(Enum::Underlying(a_lhs) | Enum::Underlying(a_rhs)); return a_lhs; } \
    SKMP_FORCEINLINE constexpr x& operator&=(x &a_lhs, x a_rhs) noexcept { \
        a_lhs = static_cast<x>(Enum::Underlying(a_lhs) & Enum::Underlying(a_rhs)); return a_lhs; } \
    SKMP_FORCEINLINE constexpr x& operator^=(x &a_lhs, x a_rhs) noexcept { \
        a_lhs = static_cast<x>(Enum::Underlying(a_lhs) ^ Enum::Underlying(a_rhs)); return a_lhs; }
