#pragma once
#include <cstdint>
#include <regex>
#include "Color.h"
#include "Utils.h"

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
            auto red = static_cast<uint8_t>(std::stoi(matches[1]));   // red value.
            auto green = static_cast<uint8_t>(std::stoi(matches[2])); // green value.
            auto blue = static_cast<uint8_t>(std::stoi(matches[3]));  // blue value.
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
    vector<pair<string, Color>> colorTable = {{"black", Color(0xff000000)}, {"blue", Color(0xff0000ff)},
                                              {"gray", Color(0xffc0c0c0)},  {"green", Color(0xff00ff00)},
                                              {"red", Color(0xffff0000)},   {"white", Color(0xffffffff)}};
    int64_t colorIndex = BinarySearch(colorTable, colorStr.c_str());
     if (colorIndex != -1) {
        return colorTable[colorIndex].second;
     }

    // parse uint32_t color string.
    auto uint32Color = StringToUint(colorStr);
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

Color Color::FromARGB(uint8_t alpha, uint8_t red, uint8_t green, uint8_t blue)
{
    ColorParam colorValue {
#if BIG_ENDIANNESS
        .argb = { .alpha = alpha, .red = red, .green = green, .blue = blue }
#else
        .argb = { .blue = blue, .green = green, .red = red, .alpha = alpha }
#endif
    };
    return Color(colorValue);
}

Color Color::FromRGBO(uint8_t red, uint8_t green, uint8_t blue, double opacity)
{
    return FromARGB(static_cast<uint8_t>(round(opacity * 0xff)) & 0xff, red, green, blue);
}

Color Color::FromRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    return FromARGB(0xff, red, green, blue);
}

Color Color::ChangeAlpha(uint8_t alpha) const
{
    return Color::FromARGB(alpha, GetRed(), GetGreen(), GetBlue());
}

} // namespace fastimage
} // namespace rnoh