//
//  TwoPointSizeConstraint.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Constraints/TwoPointSizeConstraint.h>
#include <System/Tools.h>

namespace Ice {

namespace GUI {

TwoPointSizeConstraint::TwoPointSizeConstraint(PointConstraint* pCnst1, std::unique_ptr<PointConstraint> pCnst2) : SizeConstraint{} {
    if (pCnst1 == nullptr || pCnst2 == nullptr)
        throw std::runtime_error("Passed in points must not be nullptr");

    // TODO: OnAdd Methode, dann den Parent von pCnst2 auf meinen setzen, damit default der parent this's parent Widget benutzt wird
    if (pCnst2->parent() == nullptr)
        pCnst2->setParent(pCnst1->parent());
    m_pPoint1 = pCnst1;
    m_pPoint2 = std::move(pCnst2);
}

float TwoPointSizeConstraint::getValue() const {
    return fabs(m_pPoint1->getValue() - m_pPoint2->getValue());
}

}

}
