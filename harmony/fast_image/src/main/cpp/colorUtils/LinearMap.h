/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 *
 * This file incorporates from the OpenHarmony project, licensed under
 * the Apache License, Version 2.0. Specifically:
 * - [OpenHarmony/arkui_ace_engine] (https://gitee.com/openharmony/arkui_ace_engine)
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at:
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

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
