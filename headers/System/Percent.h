//
//  Percent.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Percent_h
#define Percent_h

#include <compare>
#include <stdexcept>

namespace Ice {

class Percent {
    float m_percent;
    
public:
    constexpr Percent() = default;
    constexpr explicit Percent(float pct) {
        m_percent = pct;
    }
    
    operator float() const { return m_percent; }
    constexpr float getPercentage(float fVal) const {
        return (fVal / 100.0f) * m_percent;
    }
    constexpr auto operator<=>(const Percent&) const = default;
};

constexpr Percent operator"" _pct(long double pct) {
    return Percent{ static_cast<float>(pct) };
}

}


#endif /* Percent_h */
