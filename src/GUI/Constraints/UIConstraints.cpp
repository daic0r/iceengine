//
//  UIConstraints.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Constraints/UIConstraints.h>
#include <System/Tools.h>
#include <type_traits>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>
#include <GUI/Widget.h>

namespace Ice {

namespace GUI {

//template<typename T> requires IsMeasurementUnit<T>
//float MarginConstraint<T>::getValue() const {
//    if constexpr(std::is_same_v<T, Pixels>) {
//        const float fPix = static_cast<float>((std::uint32_t) margin());
//
//        if (direction() == OrthogonalDirection::LEFT || direction() == OrthogonalDirection::RIGHT) {
//            const float fPixWidth = 2.0f / systemServices.getGraphicsSystem()->displayWidth();
//            const float fAmount = fPixWidth * fPix;
//            switch (direction()) {
//                case OrthogonalDirection::LEFT:
//                    if (m_pParent == nullptr)
//                        return -1.0f + fAmount;
//                    return m_pParent + fAmount;
//                case OrthogonalDirection::RIGHT:
//                    if (m_pParent == nullptr)
//                        return 1.0f - fAmount;
//                    return m_pParent - fAmount;
//                default:
//                    break;
//            }
//        } else
//        if (direction() == OrthogonalDirection::TOP || direction() == OrthogonalDirection::BOTTOM) {
//            const float fPixHeight = 2.0f / systemServices.getGraphicsSystem()->displayHeight();
//            const float fAmount = fPixHeight * fPix;
//            switch (direction()) {
//                case OrthogonalDirection::BOTTOM:
//                    if (m_pParent == nullptr)
//                        return -1.0f + fAmount;
//                    return m_pParent->bottom() + fAmount;
//                case OrthogonalDirection::TOP:
//                    if (m_pParent == nullptr)
//                        return 1.0f - fAmount;
//                    return m_pParent->top() - fAmount;
//                default:
//                    break;
//            }
//        }
//    } else
//
//    if constexpr(std::is_same_v<T, Percent>) {
//        const auto fPercentage = (margin().getPercentage(2.0));
//        if (direction() == OrthogonalDirection::LEFT || direction() == OrthogonalDirection::RIGHT) {
//            switch (direction()) {
//                case OrthogonalDirection::LEFT:
//                    if (m_pParent == nullptr)
//                        return -1.0f + fPercentage;
//                    return m_pParent->left() + fPercentage;
//                case OrthogonalDirection::RIGHT:
//                    if (m_pParent == nullptr)
//                        return 1.0f - fPercentage;
//                    return m_pParent->right() - fPercentage;
//                default:
//                    break;
//            }
//        } else
//        if (direction() == OrthogonalDirection::TOP || direction() == OrthogonalDirection::BOTTOM) {
//            switch (direction()) {
//                case OrthogonalDirection::BOTTOM:
//                    if (m_pParent == nullptr)
//                        return -1.0f + fPercentage;
//                    return m_pParent->bottom() + fPercentage;
//                case OrthogonalDirection::TOP:
//                    if (m_pParent == nullptr)
//                        return 1.0f - fPercentage;
//                    return m_pParent->top() - fPercentage;
//                default:
//                    break;
//            }
//        }
//    }
//}


}

}
