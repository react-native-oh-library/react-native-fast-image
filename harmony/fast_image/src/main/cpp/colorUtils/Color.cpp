#pragma once
#include <cstdint>
#include <regex>
#include "Color.h"
#include "Utils.h"
#include "StringUtils.h"
#include "LinearMap.h"

namespace rnoh {
namespace fastimage {

namespace {

constexpr uint32_t COLOR_ALPHA_OFFSET = 24;
constexpr uint32_t COLOR_STRING_SIZE_STANDARD = 8;
constexpr uint32_t COLOR_STRING_BASE = 16;
constexpr uint32_t RGB_SUB_MATCH_SIZE = 4;
constexpr uint32_t RGBA_SUB_MATCH_SIZE = 5;

const std::regex COLOR_WITH_MAGIC("#[0-9A-Fa-f]{6,8}");
const std::regex COLOR_WITH_MAGIC_MINI("#[0-9A-Fa-f]{3,4}");
const std::regex COLOR_WITH_RGB(R"(rgb\(([0-9]{1,3})\,([0-9]{1,3})\,([0-9]{1,3})\))", std::regex::icase);
const std::regex COLOR_WITH_RGBA(R"(rgba\(([0-9]{1,3})\,([0-9]{1,3})\,([0-9]{1,3})\,(\d+\.?\d*)\))", std::regex::icase);
constexpr double GAMMA_FACTOR = 2.2;
constexpr float MAX_ALPHA = 255.0f;
constexpr char HEX[] = "0123456789ABCDEF";
constexpr uint8_t BIT_LENGTH_INT32 = 8;
constexpr uint8_t MIN_RGB_VALUE = 0;
constexpr uint8_t MAX_RGB_VALUE = 255;
constexpr double MIN_RGBA_OPACITY = 0.0;
constexpr double MAX_RGBA_OPACITY = 1.0;

} // namespace

const Color Color::TRANSPARENT = Color(0x00000000);
const Color Color::WHITE = Color(0xffffffff);
const Color Color::BLACK = Color(0xff000000);
const Color Color::RED = Color(0xffff0000);
const Color Color::GREEN = Color(0xff00ff00);
const Color Color::BLUE = Color(0xff0000ff);
const Color Color::GRAY = Color(0xffc0c0c0);
const Color Color::FOREGROUND = Color(0x00000001); // foreground color and foreground color strategy identification

Color Color::FromString(std::string colorStr, uint32_t maskAlpha, Color defaultColor) {
    if (colorStr.empty()) {
        // empty string, return transparent
        return Color::TRANSPARENT;
    }

    // Remove all " ".
    colorStr.erase(std::remove(colorStr.begin(), colorStr.end(), ' '), colorStr.end());

    std::smatch matches;
    // Regex match for #909090 or #90909090.
    if (std::regex_match(colorStr, matches, COLOR_WITH_MAGIC)) {
        colorStr.erase(0, 1);
        auto value = stoul(colorStr, nullptr, COLOR_STRING_BASE);
        if (colorStr.length() < COLOR_STRING_SIZE_STANDARD) {
            // no alpha specified, set alpha to 0xff
            value |= maskAlpha;
        }
        return Color(value);
    }
    // Regex match for #rgb or #rgba.
    if (std::regex_match(colorStr, matches, COLOR_WITH_MAGIC_MINI)) {
        colorStr.erase(0, 1);
        std::string newColorStr;
        // translate #rgb or #rgba to #rrggbb or #rrggbbaa
        for (auto &c : colorStr) {
            newColorStr += c;
            newColorStr += c;
        }
        auto value = stoul(newColorStr, nullptr, COLOR_STRING_BASE);
        if (newColorStr.length() < COLOR_STRING_SIZE_STANDARD) {
            // no alpha specified, set alpha to 0xff
            value |= maskAlpha;
        }
        return Color(value);
    }
    // Regex match for rgb(90,254,180).
    if (std::regex_match(colorStr, matches, COLOR_WITH_RGB)) {
        if (matches.size() == RGB_SUB_MATCH_SIZE) {
            auto red = static_cast<uint8_t>(std::stoi(matches[1])); // red value.
            auto green = static_cast<uint8_t>(std::stoi(matches[2])); // green value.
            auto blue = static_cast<uint8_t>(std::stoi(matches[3])); // blue value.
            return FromRGB(red, green, blue);
        }
    }
    // Regex match for rgba(90,254,180,0.5).
    if (std::regex_match(colorStr, matches, COLOR_WITH_RGBA)) {
        if (matches.size() == RGBA_SUB_MATCH_SIZE) {
            auto red = static_cast<uint8_t>(std::stoi(matches[1]));
            auto green = static_cast<uint8_t>(std::stoi(matches[2]));
            auto blue = static_cast<uint8_t>(std::stoi(matches[3]));
            auto opacity = static_cast<double>(std::stod(matches[4]));
            return FromRGBO(red, green, blue, opacity);
        }
    }
    // match for special string
    static const LinearMapNode<Color> colorTable[] = {
        {"black", Color(0xff000000)}, {"blue", Color(0xff0000ff)}, {"gray", Color(0xffc0c0c0)},
        {"green", Color(0xff00ff00)}, {"red", Color(0xffff0000)},  {"white", Color(0xffffffff)},
    };
    int64_t colorIndex = BinarySearchFindIndex(colorTable, ArraySize(colorTable), colorStr.c_str());
    if (colorIndex != -1) {
        return colorTable[colorIndex].value;
    }

    // parse uint32_t color string.
    auto uint32Color = StringUtils::StringToUint(colorStr);
    if (uint32Color > 0) {
        Color value;
        if (uint32Color >> COLOR_ALPHA_OFFSET == 0) {
            value = Color(uint32Color).ChangeAlpha(MAX_ALPHA);
        } else {
            value = Color(uint32Color);
        }
        return value;
    }

    // Default color.
    return defaultColor;
}

Color Color::FromARGB(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue) {
    ColorParam colorValue {
#if BIG_ENDIANNESS
        .argb = {.alpha = alpha, .red = red, .green = green, .blue = blue}
#else
        .argb = {.blue = blue, .green = green, .red = red, .alpha = alpha }
#endif
    };
    return Color(colorValue);
}

Color Color::FromRGBO(uint8_t red, uint8_t green, uint8_t blue, double opacity) {
    return FromARGB(static_cast<uint8_t>(round(opacity * 0xff)) & 0xff, red, green, blue);
}

Color Color::FromRGB(uint8_t red, uint8_t green, uint8_t blue) { return FromARGB(0xff, red, green, blue); }

Color Color::ChangeAlpha(uint8_t alpha) const { return Color::FromARGB(alpha, GetRed(), GetGreen(), GetBlue()); }

Color Color::operator+(const Color &color) const {
    // convert first color from ARGB to linear
    double firstLinearRed = 0.0;
    double firstLinearGreen = 0.0;
    double firstLinearBlue = 0.0;
    ConvertGammaToLinear(*this, firstLinearRed, firstLinearGreen, firstLinearBlue);

    // convert second color from ARGB to linear
    double secondLinearRed = 0.0;
    double secondLinearGreen = 0.0;
    double secondLinearBlue = 0.0;
    ConvertGammaToLinear(color, secondLinearRed, secondLinearGreen, secondLinearBlue);

    // get linear result and convert to gamma
    return ConvertLinearToGamma(GetAlpha() + color.GetAlpha(), firstLinearRed + secondLinearRed,
                                firstLinearGreen + secondLinearGreen, firstLinearBlue + secondLinearBlue);
}

Color Color::operator-(const Color &color) const {
    // convert first color from ARGB to linear
    double firstLinearRed = 0.0;
    double firstLinearGreen = 0.0;
    double firstLinearBlue = 0.0;
    ConvertGammaToLinear(*this, firstLinearRed, firstLinearGreen, firstLinearBlue);

    // convert second color from ARGB to linear
    double secondLinearRed = 0.0;
    double secondLinearGreen = 0.0;
    double secondLinearBlue = 0.0;
    ConvertGammaToLinear(color, secondLinearRed, secondLinearGreen, secondLinearBlue);

    // get linear result and convert to gamma
    return ConvertLinearToGamma(GetAlpha() - color.GetAlpha(), firstLinearRed - secondLinearRed,
                                firstLinearGreen - secondLinearGreen, firstLinearBlue - secondLinearBlue);
}

Color Color::operator*(double value) const {
    // convert color from ARGB to linear
    double linearRed = 0.0;
    double linearGreen = 0.0;
    double linearBlue = 0.0;
    ConvertGammaToLinear(*this, linearRed, linearGreen, linearBlue);

    // get linear result and convert to gamma
    return ConvertLinearToGamma(GetAlpha() * value, linearRed * value, linearGreen * value, linearBlue * value);
}

Color Color::operator/(double value) const {
    if (NearZero(value)) {
        return *this;
    }
    // convert color from ARGB to linear
    double linearRed = 0.0;
    double linearGreen = 0.0;
    double LinearBlue = 0.0;
    ConvertGammaToLinear(*this, linearRed, linearGreen, LinearBlue);

    // get linear result and convert to gamma
    return ConvertLinearToGamma(GetAlpha() / value, linearRed / value, linearGreen / value, LinearBlue / value);
}

double Color::ConvertGammaToLinear(uint8_t value) { return std::pow(value, GAMMA_FACTOR); }

uint8_t Color::ConvertLinearToGamma(double value) {
    return std::clamp(static_cast<int32_t>(round(std::pow(value, 1.0 / GAMMA_FACTOR))), 0, UINT8_MAX);
}

void Color::ConvertGammaToLinear(const Color &gammaColor, double &linearRed, double &linearGreen, double &linearBlue) {
    linearRed = ConvertGammaToLinear(gammaColor.GetRed());
    linearGreen = ConvertGammaToLinear(gammaColor.GetGreen());
    linearBlue = ConvertGammaToLinear(gammaColor.GetBlue());
}

Color Color::ConvertLinearToGamma(double alpha, double linearRed, double linearGreen, double linearBlue) {
    uint8_t gammaRed = ConvertLinearToGamma(linearRed);
    uint8_t gammaGreen = ConvertLinearToGamma(linearGreen);
    uint8_t gammaBlue = ConvertLinearToGamma(linearBlue);
    uint8_t gammaAlpha = std::clamp(static_cast<int32_t>(round(alpha)), 0, UINT8_MAX);

    return FromARGB(gammaAlpha, gammaRed, gammaGreen, gammaBlue);
}

} // namespace fastimage
} // namespace rnoh