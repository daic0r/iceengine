//
//  Padding.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 16.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Padding.h>
#include <System/Percent.h>
#include <GUI/Widget.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

namespace GUI {

Padding::Padding() {
    m_mPaddings[OrthogonalDirection::LEFT] = 0_px;
    m_mPaddings[OrthogonalDirection::RIGHT] = 0_px;
    m_mPaddings[OrthogonalDirection::BOTTOM] = 0_px;
    m_mPaddings[OrthogonalDirection::TOP] = 0_px;
}

Padding::Padding(float fLeft, float fTop, float fRight, float fBottom) {
    auto pG = systemServices.getGraphicsSystem();
    m_mPaddings[OrthogonalDirection::LEFT] = Pixels( fLeft / pG->getHorizontalPixelScale() );
    m_mPaddings[OrthogonalDirection::RIGHT] = Pixels( fRight / pG->getHorizontalPixelScale() );
    m_mPaddings[OrthogonalDirection::BOTTOM] = Pixels( fBottom / pG->getVerticalPixelScale() );
    m_mPaddings[OrthogonalDirection::TOP] = Pixels( fTop / pG->getVerticalPixelScale() );
}

Padding::Padding(Pixels left, Pixels top, Pixels right, Pixels bottom) {
    setPadding(OrthogonalDirection::LEFT, left);
    setPadding(OrthogonalDirection::RIGHT, right);
    setPadding(OrthogonalDirection::TOP, top);
    setPadding(OrthogonalDirection::BOTTOM, bottom);
}

Pixels Padding::padding(OrthogonalDirection dir) const {
    if (dir == OrthogonalDirection::NONE)
        throw std::runtime_error("Cannot get padding for direction NONE");
    
    return m_mPaddings.at(dir);
}

void Padding::setPadding(OrthogonalDirection dir, Pixels padding) {
    if (dir == OrthogonalDirection::NONE)
        throw std::runtime_error("Cannot set padding for direction NONE");
    
    m_mPaddings[dir] = padding;
}

Padding DefaultPaddingCalculator::operator()() const noexcept {
    static constexpr auto p = 3.0_pct;
    static constexpr auto defaultPercentage = p.getPercentage(2.0f);
    
    const float fLeft = widget() != nullptr ? p.getPercentage(widget()->width()) : defaultPercentage;
    const float fTop = widget() != nullptr ? p.getPercentage(widget()->height()) : defaultPercentage;
    const float fRight = widget() != nullptr ? p.getPercentage(widget()->width()) : defaultPercentage;
    const float fBottom = widget() != nullptr ? p.getPercentage(widget()->height()) : defaultPercentage;
    return Padding{ fLeft, fTop, fRight, fBottom };
}

}

}
