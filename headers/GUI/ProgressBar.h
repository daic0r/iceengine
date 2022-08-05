//
//  ProgressBar.h
//  IceCraft
//
//  Created by Matthias Gruen on 25.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ProgressBar_h
#define ProgressBar_h

#include <GUI/Widget.h>
#include <System/Percent.h>

namespace Ice {

namespace GUI {

class ProgressBar : public Widget {
    friend class Slider;
    
    float m_fMinValue{ 0.0f };
    float m_fMaxValue{ 100.0f };
    float m_fProgress{ 0.0f };
    Widget* m_pProgressBarWidget{ nullptr };
    
public:
    // Size constraint is for the contained widget that will display the actual progress
    ProgressBar(std::unique_ptr<SizeConstraint> pVertical = nullptr);
    
    auto minValue() const noexcept { return m_fMinValue; }
    void setMinValue(float f) noexcept { m_fMinValue = f; }
    auto maxValue() const noexcept { return m_fMaxValue; }
    void setMaxValue(float f) noexcept { m_fMaxValue = f; }
    auto range() const noexcept { return maxValue() - minValue(); }
    
    auto progressBarColor() const noexcept { return m_pProgressBarWidget->color(); }
    void setProgressBarColor(const glm::vec4& c) noexcept { m_pProgressBarWidget->setColor(c); }
    
    auto progress() const noexcept { return m_fProgress; }
    void setProgress(Percent p) noexcept;
    void setProgress(float p) noexcept;
    
};

}

}


#endif /* ProgressBar_h */
