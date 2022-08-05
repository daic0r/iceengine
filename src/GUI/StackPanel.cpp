//
//  StackPanel.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/StackPanel.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <System/Percent.h>
#include <System/Pixels.h>

namespace Ice {

namespace GUI {

//StackPanel<OrthogonalAxis::VERTICAL>::StackPanel<OrthogonalAxis::VERTICAL>() {
//    setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
//    setPadding(0.0f, 0.0f, 0.0f, 0.0f);
//}
//
///*
// * \name addChild
// *
// * \description Adds the child widget and additionally sets position and size constraints on it
// *              to fit the size of the stack panel
// *
// * \date 21.12.2020
// */
//void StackPanel<OrthogonalAxis::VERTICAL>::addChild(Widget* w, size_t nDrawOrder) {
//
//    // Added child is aligned to the left, as wide as the stack panel and will
//    // be aligned with the bottom of the preceding element
//    w->setParent(this);
//    w->setPointConstraint(HorizontalAlignment::CENTER, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::CENTER));
//    if (children().size() == 0)
//        w->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
//    else
//        w->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM, false, children().back().pChild));
//    w->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
//
//    Widget::addChild(w, nDrawOrder);
//
//    auto pVertSizeConstraint = getSizeConstraint(OrthogonalAxis::VERTICAL);
//    // If we have no size constraint or it's a dynamic one (set by the code below in a previous call)
//    // then we're automatically size and will grow along with the size of the children...
//    if (pVertSizeConstraint == nullptr || dynamic_cast<DynamicSizeConstraint*>(pVertSizeConstraint) != nullptr) {
//        setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
//            float fHeight = 0.0f;
//            for (const auto& info : w->children()) {
//                fHeight += info.child()->height();
//            }
//            return fHeight;
//        }, true, this));
//    } else {
//    // ... if we have a fixed size, resize all children to have the same size to fit in the panel
//        for (auto& info : children()) {
//            info.child()->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<NormalizedPixels>>(NormalizedPixels{ height() / (float)children().size() }));
//            info.child()->refreshConstraints();
//        }
//    }
//}

}

}
