#pragma once
#include <chrono>
#include <cmath>
#include <cstdint>

#define CHECK_NULL_VOID(ptr)                                                                                           \
    do {                                                                                                               \
        if (!(ptr)) {                                                                                                  \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

#define CHECK_NULL_RETURN(ptr, ret)                                                                                    \
    do {                                                                                                               \
        if (!(ptr)) {                                                                                                  \
            return ret;                                                                                                \
        }                                                                                                              \
    } while (0)

#define CHECK_ERROR_CODE_RETURN(code)                                                                                  \
    do {                                                                                                               \
        if ((code) > 0) {                                                                                              \
            return code;                                                                                               \
        }                                                                                                              \
    } while (0)

#define CHECK_EQUAL_VOID(var, value)                                                                                   \
    do {                                                                                                               \
        if ((var) == (value)) {                                                                                        \
            return;                                                                                                    \
        }                                                                                                              \
    } while (0)

namespace rnoh {
namespace fastimage{

template<typename T, std::size_t N>
constexpr std::size_t ArraySize(T (&)[N]) noexcept
{
    return N;
}

inline bool NearEqual(const double left, const double right, const double epsilon)
{
    return (std::abs(left - right) <= epsilon);
}

template<typename T>
constexpr bool NearEqual(const T& left, const T& right);

inline bool NearZero(const double value, const double epsilon)
{
    return NearEqual(value, 0.0, epsilon);
}

inline bool NearZero(const double left)
{
    constexpr double epsilon = 0.001f;
    return NearZero(left, epsilon);
}

} // namespace fastimage
} // namespace rnoh