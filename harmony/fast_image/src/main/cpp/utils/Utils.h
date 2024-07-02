#pragma once
#include "Color.h"
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
#include <string>

using namespace std;

namespace rnoh {
namespace fastimage{

inline int BinarySearch(vector<pair<string, Color>> &colors, const string &targetColor) {
    int left = 0;
    int right = colors.size() - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (colors[mid].first == targetColor) {
            return mid;
        } else if (colors[mid].first < targetColor) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

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

} // namespace fastimage
} // namespace rnoh