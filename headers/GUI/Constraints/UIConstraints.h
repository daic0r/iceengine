//
//  UIConstraints.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef UIConstraints_h
#define UIConstraints_h

#include <IceEngine.h>
#include <Interfaces/IUIConstraint.h>
#include <System/Types.h>
#include <System/Pixels.h>
#include <System/Percent.h>
#include <Utils/TemplateHelpers.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

namespace GUI {

class Widget;

enum class HorizontalAlignment {
    NONE,
    LEFT,
    CENTER,
    RIGHT
};

enum class VerticalAlignment {
    NONE,
    BOTTOM,
    MIDDLE,
    TOP
};

class ICEENGINE_API ABaseConstraint : public IUIConstraint {
    Widget* m_pParent{ nullptr };   // parent of the widget this constraint is added to
    bool m_bHonorPadding{ true };
    
public:
    //ABaseConstraint() noexcept = default;
    ABaseConstraint(bool b = true, Widget *w = nullptr) noexcept : m_pParent{ w }, m_bHonorPadding{ b } {}
    auto parent() const noexcept { return m_pParent; }
    void setParent(Widget* w) noexcept { m_pParent = w; }
    auto honorPadding() const noexcept { return m_bHonorPadding; }
    void setHonorPadding(bool b) noexcept { m_bHonorPadding = b; }
    bool dynamic() const noexcept override { return false; }
};

class ICEENGINE_API PointConstraint : public ABaseConstraint {

public:
    PointConstraint(bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : ABaseConstraint{ bHonorPadding, pParent } {}
};

class ICEENGINE_API SizeConstraint : public ABaseConstraint {
    OrthogonalAxis m_axis;
public:
    SizeConstraint(OrthogonalAxis axis = OrthogonalAxis::NONE, bool bHonorPadding = true, Widget* pParent = nullptr) noexcept : m_axis{ axis }, ABaseConstraint{ bHonorPadding, pParent } {}
    auto axis() const noexcept { return m_axis; }
    void setAxis(OrthogonalAxis a) noexcept { m_axis = a; }
};


}   // namespace GUI

}   // namespace Ice

#endif /* UIConstraints_h */
