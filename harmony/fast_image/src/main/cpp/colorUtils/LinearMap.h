// from ArkUI "frameworks/base/utils/linear_map.h"
#pragma once

#include <algorithm>
#include <cstring>

namespace rnoh {
namespace fastimage {

template <typename T> struct LinearMapNode {
    const char *key;
    T value;
};

// the key type K must can be compared.
template <typename K, typename V> struct LinearEnumMapNode {
    K key;
    V value;
};

// these binary search functions in linear map, so the map must be sorted by key.
template <typename T> int64_t BinarySearchFindIndex(const LinearMapNode<T> *vec, size_t length, const char *key) {
    auto it = std::lower_bound(vec, vec + length, key,
                               [](LinearMapNode<T> lhs, const char *key) { return strcmp(lhs.key, key) < 0; });
    if (it == vec + length || strcmp(it->key, key) != 0) {
        return -1;
    } else {
        return it - vec;
    }
}

} // namespace fastimage
} // namespace rnoh
