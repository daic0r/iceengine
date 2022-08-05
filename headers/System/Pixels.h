//
//  Pixels.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Pixels_h
#define Pixels_h

#include <compare>

namespace Ice {


template<typename T>
class BasePixels {
    T m_pixels{ 0 };
public:
    constexpr BasePixels() = default;
    constexpr explicit BasePixels(T px) : m_pixels{ px } {}
    
    operator T() const { return m_pixels; }
    constexpr auto operator<=>(const BasePixels&) const = default;
};

using Pixels = BasePixels<std::uint32_t>;
using NormalizedPixels = BasePixels<float>;


//class Pixels {
//
//    std::uint32_t m_pixels{ 0 };
//    bool m_bNormalized{ false };
//public:
//    constexpr Pixels() = default;
//    constexpr explicit Pixels(std::uint32_t px) : m_pixels{ px } {}
//
//    operator std::uint32_t() const { return m_pixels; }
//    constexpr auto operator<=>(const Pixels&) const = default;
//};

constexpr Pixels operator"" _px(unsigned long long px) {
    return Pixels(static_cast<std::uint32_t>(px));
}

constexpr NormalizedPixels operator"" _npx(long double px) {
    return NormalizedPixels(static_cast<float>(px));
}

}


#endif /* Pixels_h */
