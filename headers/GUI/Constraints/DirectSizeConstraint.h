//
//  DirectSizeConstraint.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef DirectSizeConstraint_h
#define DirectSizeConstraint_h

#include <GUI/Constraints/UIConstraints.h>
#include <Utils/TemplateHelpers.h>
#include <System/Pixels.h>
#include <System/Percent.h>
#include <iostream>

namespace Ice {

namespace GUI {

template<typename T> requires IsMeasurementUnit<T>
class DirectSizeConstraint : public SizeConstraint {
    T m_size;

public:
    DirectSizeConstraint(T size, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : SizeConstraint{ OrthogonalAxis::NONE, bHonorPadding, pParent }, m_size{ size } {}
    T size() const noexcept { return m_size; }
    
    void setSize(T val) noexcept {
        m_size = val;
        if (parent() != nullptr) {
            parent()->setGraphicsDirty(true);
        }
    }
    
    float getValue() const override {
        if constexpr(std::is_same_v<T, Pixels> || std::is_same_v<T, NormalizedPixels>) {
            float fPix;
            if constexpr(std::is_same_v<T, Pixels>)
                fPix = static_cast<float>((std::uint32_t) size());
            else
            if constexpr(std::is_same_v<T, NormalizedPixels>)
                fPix = static_cast<float>(size());
            
            float fAmount;
            if constexpr(std::is_same_v<T, Pixels>) {
                const float fPixDim = (axis() == OrthogonalAxis::HORIZONTAL ? systemServices.getGraphicsSystem()->getHorizontalPixelScale(): systemServices.getGraphicsSystem()->getVerticalPixelScale());
                fAmount = fPixDim * fPix;
            } else
            if constexpr(std::is_same_v<T, NormalizedPixels>)
                fAmount = fPix;

            return fAmount;
        } else
        if constexpr(std::is_same_v<T, Percent>) {
            float fPercentageBase;
            if (parent() != nullptr) {
                switch (axis()) {
                    case OrthogonalAxis::HORIZONTAL:
                        fPercentageBase = parent()->right(honorPadding()) - parent()->left(honorPadding());
                        break;
                    case OrthogonalAxis::VERTICAL:
                        fPercentageBase = parent()->top(honorPadding()) - parent()->bottom(honorPadding());
                        break;
                }
            } else {
                fPercentageBase = 2.0f;
            }
            const float fAmount = size().getPercentage(fPercentageBase);
            return fAmount;
        }
        return -1.0f;
    }
};


}

}


#endif /* DirectSizeConstraint_h */
