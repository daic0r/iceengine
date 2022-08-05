//
//  ProgressBar.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 25.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/ProgressBar.h>
#include <System/SystemServices.h>
#include <GUI/WidgetManager.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/TwoPointSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>

namespace Ice {

namespace GUI {

ProgressBar::ProgressBar(std::unique_ptr<SizeConstraint> pVertical) {

    auto& wm = systemServices.getWidgetManager();
    auto pProgress = wm.createWidget<Widget, true>(this);
    
    pProgress->setColor(glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f });
    pProgress->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    pProgress->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    if (!pVertical)
        pProgress->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(pProgress->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
    else {
        pVertical->setParent(this);
        pProgress->setSizeConstraint(OrthogonalAxis::VERTICAL, std::move(pVertical));
    }
    pProgress->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
        ProgressBar* pParent = static_cast<ProgressBar*>(w);
        return pParent->contentWidth() * ((pParent->progress() - pParent->minValue()) / (pParent->maxValue() - pParent->minValue()));
    }));
    m_pProgressBarWidget = pProgress;
}

void ProgressBar::setProgress(Percent p) noexcept {
    setProgress(minValue() + ((maxValue() - minValue()) * (std::min(100.0f, std::max(0.0f, (float)p)) / 100.0f)));
}

void ProgressBar::setProgress(float p) noexcept {
    m_fProgress = std::min(maxValue(), std::max(minValue(), p));
    m_pProgressBarWidget->refreshConstraints();
}

}

}
