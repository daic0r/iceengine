//
//  DynamicConstraint.h
//  IceCraft
//
//  Created by Matthias Gruen on 22.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef DynamicConstraint_h
#define DynamicConstraint_h

#include <IceEngine.h>
#include <GUI/Constraints/UIConstraints.h>
#include <System/Types.h>
#include <functional>

namespace Ice {

namespace GUI {

class ICEENGINE_API DynamicPointConstraint : public PointConstraint {
    using CallableType = std::function<float(Widget*)>;
    
    CallableType m_func;
    
public:
    DynamicPointConstraint(const CallableType& func, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : PointConstraint{ bHonorPadding, pParent }, m_func{ func } {}
    
    bool dynamic() const noexcept override { return true; }
    float getValue() const override {
        return m_func(parent());
    }
};

class DynamicSizeConstraint : public SizeConstraint {
    using CallableType = std::function<float(Widget*)>;
    
    CallableType m_func;
    
public:
    DynamicSizeConstraint(const CallableType& func, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : SizeConstraint{ OrthogonalAxis::NONE, bHonorPadding, pParent }, m_func{ func } {}
    
    bool dynamic() const noexcept override { return true; }
    float getValue() const override {
        return m_func(parent());
    }
    
};

}
}

#endif /* DynamicConstraint_h */
