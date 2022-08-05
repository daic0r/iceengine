//
//  Padding.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Padding_h
#define Padding_h

#include <System/Types.h>
#include <unordered_map>
#include <System/Pixels.h>

namespace Ice {

namespace GUI {

class Widget;

class Padding {
    std::unordered_map<OrthogonalDirection, Pixels> m_mPaddings;
    
public:
    
    Padding();
    Padding(float, float, float, float); // left, top, right, bottom
    Padding(Pixels, Pixels, Pixels, Pixels);
    Pixels padding(OrthogonalDirection dir) const;
    void setPadding(OrthogonalDirection dir, Pixels);
};

class APaddingCalculator {
    Widget* m_pWidget{ nullptr };
    
public:
    APaddingCalculator() = delete;
    APaddingCalculator(Widget* w) : m_pWidget{ w } {}
    auto widget() const noexcept { return m_pWidget; }
    virtual Padding operator()() const noexcept = 0;
};

class DefaultPaddingCalculator : public APaddingCalculator {
public:
    
    DefaultPaddingCalculator(Widget* w) : APaddingCalculator{ w } {}
    Padding operator()() const noexcept override;
};

}

}


#endif /* Padding_h */
