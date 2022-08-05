//
//  MarginConstraint.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MarginConstraint_h
#define MarginConstraint_h

#include <System/Pixels.h>
#include <System/Percent.h>
#include <GUI/Widget.h>
#include <System/Types.h>
#include <Utils/TemplateHelpers.h>

namespace Ice {

namespace GUI {

template<typename T> requires IsMeasurementUnit<T>
class MarginConstraint : public PointConstraint {
    OrthogonalDirection m_dir;
    T m_margin;
    
public:
    MarginConstraint(OrthogonalDirection dir, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : PointConstraint{ bHonorPadding, pParent }, m_dir{ dir } {}
    MarginConstraint(OrthogonalDirection dir, T margin, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : PointConstraint{ bHonorPadding, pParent }, m_dir{ dir } {
        m_margin = margin;
    }
    T margin() const noexcept { return m_margin; }
    auto direction() const noexcept { return m_dir; }

    void setMargin(T val) noexcept {
        m_margin = val;
        if (parent() != nullptr) {
            parent()->refreshConstraints();
        }
    }
    
    float getValue() const override {
        if constexpr(std::is_same_v<T, Pixels> || std::is_same_v<T, NormalizedPixels>) {
            float fPix;
            
            if constexpr(std::is_same_v<T, Pixels>)
                fPix = static_cast<float>((std::uint32_t) margin());
            else
            if constexpr(std::is_same_v<T, NormalizedPixels>)
                fPix = static_cast<float>(margin());
            
            if (direction() == OrthogonalDirection::LEFT || direction() == OrthogonalDirection::RIGHT) {
                float fAmount;
                if constexpr(std::is_same_v<T, Pixels>) {
                    const float fPixWidth = systemServices.getGraphicsSystem()->getHorizontalPixelScale();
                    fAmount = fPixWidth * fPix;
                }
                else
                if constexpr(std::is_same_v<T, NormalizedPixels>)
                    fAmount = fPix;
                
                if (parent() != nullptr)
                    fAmount = std::min(fAmount, parent()->width() / 2.0f);
                
                switch (direction()) {
                    case OrthogonalDirection::LEFT:
                        if (parent() == nullptr)
                            return -1.0f + fAmount;
                        return parent()->left(honorPadding()) + fAmount;
                    case OrthogonalDirection::RIGHT:
                        if (parent() == nullptr)
                            return 1.0f - fAmount;
                        return parent()->right(honorPadding()) - fAmount;
                    default:
                        return -1.0f;
                }
            } else
            if (direction() == OrthogonalDirection::TOP || direction() == OrthogonalDirection::BOTTOM) {
                float fAmount;
                if constexpr(std::is_same_v<T, Pixels>) {
                    const float fPixHeight = systemServices.getGraphicsSystem()->getVerticalPixelScale();
                    fAmount = fPixHeight * fPix;
                }
                else
                if constexpr(std::is_same_v<T, NormalizedPixels>)
                    fAmount = fPix;

                if (parent() != nullptr)
                    fAmount = std::min(fAmount, parent()->height() / 2.0f);
                
                switch (direction()) {
                    case OrthogonalDirection::BOTTOM:
                        if (parent() == nullptr)
                            return -1.0f + fAmount;
                        return parent()->bottom(honorPadding()) + fAmount;
                    case OrthogonalDirection::TOP:
                        if (parent() == nullptr)
                            return 1.0f - fAmount;
                        return parent()->top(honorPadding()) - fAmount;
                    default:
                        return -1.0f;
                }
            }
        } else
        
        if constexpr(std::is_same_v<T, Percent>) {
            if (direction() == OrthogonalDirection::LEFT || direction() == OrthogonalDirection::RIGHT) {
                const auto fPercentage = parent() == nullptr ? margin().getPercentage(2.0) : margin().getPercentage(parent()->width());
                if (parent() != nullptr)
                    fPercentage = std::min(fPercentage, parent()->width() / 2.0f);
                switch (direction()) {
                    case OrthogonalDirection::LEFT:
                        if (parent() == nullptr)
                            return -1.0f + fPercentage;
                        return parent()->left(honorPadding()) + fPercentage;
                    case OrthogonalDirection::RIGHT:
                        if (parent() == nullptr)
                            return 1.0f - fPercentage;
                        return parent()->right(honorPadding()) - fPercentage;
                    default:
                        return -1.0f;
                }
            } else
            if (direction() == OrthogonalDirection::TOP || direction() == OrthogonalDirection::BOTTOM) {
                const auto fPercentage = parent() == nullptr ? margin().getPercentage(2.0) : margin().getPercentage(parent()->height());
                if (parent() != nullptr)
                    fPercentage = std::min(fPercentage, parent()->height() / 2.0f);
                switch (direction()) {
                    case OrthogonalDirection::BOTTOM:
                        if (parent() == nullptr)
                            return -1.0f + fPercentage;
                        return parent()->bottom(honorPadding()) + fPercentage;
                    case OrthogonalDirection::TOP:
                        if (parent() == nullptr)
                            return 1.0f - fPercentage;
                        return parent()->top(honorPadding()) - fPercentage;
                    default:
                        return -1.0f;
                }
            }
        }
        
        return -1.0f;
    }
};

}

}


#endif /* MarginConstraint_h */
