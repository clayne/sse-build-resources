#pragma once

#include <type_traits>

namespace Enum
{
    template <typename T>
    constexpr typename std::underlying_type<T>::type Underlying(T e) noexcept {
        return static_cast<typename std::underlying_type<T>::type>(e);
    }
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
