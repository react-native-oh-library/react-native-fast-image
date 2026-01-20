// from ArkUI "frameworks/base/utils/string_utils.h"
/*
 * Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved
 * Use of this source code is governed by a MIT license that can be
 * found in the LICENSE file.
 */
 
#pragma once
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace rnoh {
namespace fastimage {
namespace StringUtils {

inline uint32_t StringToUint(const std::string &value, uint32_t defaultErr = 0) {
    errno = 0;
    char *pEnd = nullptr;
    uint64_t result = std::strtoull(value.c_str(), &pEnd, 10);
    if (pEnd == value.c_str() || result > UINT32_MAX || errno == ERANGE) {
        return defaultErr;
    } else {
        return result;
    }
}

} // namespace StringUtils
} // namespace fastimage
} // namespace rnoh
