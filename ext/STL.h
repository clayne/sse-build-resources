#pragma once

#include "Mem.h"
#include "Hash.h"

#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>

namespace stl
{
    template <class K, class V, class H = std::hash<K>, class P = std::equal_to<K>, class A = mem::aligned_allocator<std::pair<const K, V>, 32>>
    using unordered_map = std::unordered_map<K, V, H, P, A>;

    template <class K, class V, class C = std::less<K>, class A = mem::aligned_allocator<std::pair<const K, V>, 32>>
    using map = std::map<K, V, C, A>;

    template <class K, class H = std::hash<K>, class E = std::equal_to<K>, class A = mem::aligned_allocator<K, 32>>
    using unordered_set = std::unordered_set<K, H, E, A>;

    template <class K, class C = std::less<K>, class A = mem::aligned_allocator<K, 32>>
    using set = std::set<K, C, A>;

    template <class V, class A = mem::aligned_allocator<V, 32>>
    using vector = std::vector<V, A>;

    template <class K, class V, class A = mem::aligned_allocator<std::pair<const K, V>, 32>>
    using iunordered_map = std::unordered_map<K, V, hash::i_fnv_1a, hash::iequal_to, A>;

    template <class K, class V, class A = mem::aligned_allocator<std::pair<const K, V>, 32>>
    using imap = std::map<K, V, hash::icase_comp, A>;

    template <class K, class A = mem::aligned_allocator<K, 32>>
    using iunordered_set = std::unordered_set<K, hash::i_fnv_1a, hash::iequal_to, A>;

    template <class K, class A = mem::aligned_allocator<K, 32>>
    using iset = std::set<K, hash::icase_comp, A>;

}
