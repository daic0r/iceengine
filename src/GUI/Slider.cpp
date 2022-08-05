//
//  Slider.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 26.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Slider.h>

#include <GUI/ProgressBar.h>
#include <GUI/WidgetManager.h>
#include <System/SystemServices.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <iostream>


namespace Ice {

namespace GUI {

Slider::Slider() {
    setIsTabStoppable(true);

    auto& wm = systemServices.getWidgetManager();
    m_pProgressBar = static_cast<ProgressBar*>(wm.createWidget<ProgressBar, true>(this, std::make_unique<DirectSizeConstraint<Pixels>>(1_px)));
    m_pProgressBar->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    m_pProgressBar->setPointConstraint(VerticalAlignment::MIDDLE, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::MIDDLE));
    m_pProgressBar->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    m_pProgressBar->setProgressBarColor(glm::vec4{ 1, 1, 1, 1 });
    m_pProgressBar->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    // ^^^
    // Padding must be zero. We're only one pixel high so the padding inside would be bigger than our actual size,
    // so querying values honoring padding would yield nonsensical output
    m_pProgressBar->m_fProgress = 100.0f;
    // ^^^
    // Set the progress here directly, because we don't want refreshConstraints() to be triggered here
    // This will lead to an exception, since the Slider is just being constructed and the constraints
    // cannot have been set yet by the calling code
    
    m_pSlider = wm.createWidget<Widget, true>(this);
    
    m_pSlider->setPointConstraint(HorizontalAlignment::CENTER, std::make_unique<DynamicPointConstraint>([](Widget *w){
        Slider *pParent = static_cast<Slider*>(w);
        return pParent->sliderPosition();
    }));
    m_pSlider->setPointConstraint(VerticalAlignment::MIDDLE, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::MIDDLE));
    m_pSlider->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(Pixels( 6 )));
    m_pSlider->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(80.0_pct));
    m_pSlider->setColor(glm::vec4{ 1.0f, 0.0f, 0.0f, .8f });
    m_pSlider->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    
    m_pSlider->setOnMouseOverFunc([](Widget* w, float x, float y, float offsetX, float offsetY, std::uint8_t buttonState, bool& bHandled) {
        if (buttonState & (std::uint8_t)MouseButton::LEFT) {
            Slider *pSlider = static_cast<Slider*>(w->parent());
            const auto fFactor = pSlider->range() / pSlider->m_pProgressBar->contentWidth();
            pSlider->setValue(pSlider->value() + offsetX * fFactor);
            w->refreshConstraints();
        }
    });
}

void Slider::setValue(float f) noexcept {
    m_fValue = std::min(maxValue(), std::max(minValue(), f));
}

float Slider::sliderPosition() const noexcept {
    const float fFactor = m_pProgressBar->contentWidth() / range();
    const float fRet = m_pProgressBar->left() + (fFactor * (value() - minValue()));
    return fRet;
}

glm::vec4 Slider::rulerColor() const noexcept {
    return m_pProgressBar->color();
}

void Slider::setRulerColor(const glm::vec4& c) noexcept {
    m_pProgressBar->setColor(c);
}

glm::vec4 Slider::sliderColor() const noexcept {
    return m_pSlider->color();
}

void Slider::setSliderColor(const glm::vec4& c) noexcept {
    m_pSlider->setColor(c);
}

}

}
