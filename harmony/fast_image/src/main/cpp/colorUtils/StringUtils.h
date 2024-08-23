// from ArkUI "frameworks/base/utils/string_utils.h"
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
