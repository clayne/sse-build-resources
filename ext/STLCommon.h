#pragma once

#include <type_traits>

namespace stl
{
    template <typename T>
    struct remove_all_pointers {
    public:
        using type = T;
    };

    template <typename T>
    struct remove_all_pointers<T*> {
    public:
        using type = typename remove_all_pointers<T>::type;
    };

    template <class T>
    using remove_all_pointers_t = typename remove_all_pointers<T>::type;

    template <class T>
    using strip_type = std::remove_cv_t<std::remove_pointer_t<std::remove_all_extents_t<remove_all_pointers_t<std::remove_reference_t<T>>>>>;

    template <class _Ty, class... _Types>
    inline constexpr bool is_any_base_of_v =
        std::disjunction_v<std::is_base_of<_Types, _Ty>...>;

    template <class _Ty, class... _Types>
    inline constexpr bool is_any_same_v =
        std::disjunction_v<std::is_same<_Types, _Ty>...>;

	/*template <class T>
	using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;*/

}