//
//  TwoPointSizeConstraint.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TwoPointSizeConstraint_h
#define TwoPointSizeConstraint_h

#include <IceEngine.h>
#include <GUI/Constraints/UIConstraints.h>

namespace Ice {

namespace GUI {

class TwoPointSizeConstraint : public SizeConstraint {
  
    PointConstraint* m_pPoint1{ nullptr };
    std::unique_ptr<PointConstraint> m_pPoint2;
    
public:
    
    TwoPointSizeConstraint(PointConstraint* pCnst1, std::unique_ptr<PointConstraint> pCnst2);
    float getValue() const override;
};

}

}

#endif /* TwoPointSizeConstraint_h */
