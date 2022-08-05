//
//  AlignmentConstraint.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AlignmentConstraint_h
#define AlignmentConstraint_h

#include <IceEngine.h>
#include <GUI/Constraints/UIConstraints.h>
#include <System/Types.h>

namespace Ice {

namespace GUI {

class ICEENGINE_API HorizontalAlignmentConstraint : public PointConstraint {
    HorizontalAlignment m_align;
    
public:
    HorizontalAlignmentConstraint(HorizontalAlignment align, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : PointConstraint{ bHonorPadding, pParent }, m_align{ align } {}
    float getValue() const override;
};

class ICEENGINE_API VerticalAlignmentConstraint : public PointConstraint {
    VerticalAlignment m_align;
    
public:
    VerticalAlignmentConstraint(VerticalAlignment align, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : PointConstraint{ bHonorPadding, pParent }, m_align{ align } {}
    float getValue() const override;
};

}

}


#endif /* AlignmentConstraint_h */
