#ifndef RGB_h
#define RGB_h

#include <glm/vec4.hpp>

namespace Ice {
    struct ColorComponent {
        unsigned char c{};

        constexpr ColorComponent(unsigned char col = 0) : c{ col } {} 
        constexpr operator float() const noexcept {
            return c / 255.0f;
        }
    };

    struct RGBA {
        ColorComponent r{}, g{}, b{}, a{ 255 };

        constexpr operator glm::vec4() const noexcept {
            return glm::vec4{ float{ r }, float{ g }, float{ b }, float{ a } };
        }
    };
}

#endif