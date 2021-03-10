#pragma once

#include <type_traits>

namespace stl
{
    template <class T>
    using strip_type = std::remove_all_extents_t<std::remove_pointer_t<std::remove_reference_t<std::remove_cv_t<T>>>>;

    template <class _Ty, class... _Types>
    inline constexpr bool is_any_base_of_v =
        std::disjunction_v<std::is_base_of<_Types, _Ty>...>;
}