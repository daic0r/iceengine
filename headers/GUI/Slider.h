//
//  Slider.h
//  IceCraft
//
//  Created by Matthias Gruen on 26.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Slider_h
#define Slider_h

#include <GUI/Widget.h>

namespace Ice {

namespace GUI {

class ProgressBar;

class Slider : public Widget {
public:
    Slider();
    
    float value() const noexcept { return m_fValue; }
    void setValue(float) noexcept;
    
    auto minValue() const noexcept { return m_fMinValue; }
    void setMinValue(float f) noexcept { m_fMinValue = f; }
    auto maxValue() const noexcept { return m_fMaxValue; }
    void setMaxValue(float f) noexcept { m_fMaxValue = f; }
    auto range() const noexcept { return maxValue() - minValue(); }
    
    glm::vec4 rulerColor() const noexcept;
    void setRulerColor(const glm::vec4&) noexcept;
    glm::vec4 sliderColor() const noexcept;
    void setSliderColor(const glm::vec4&) noexcept;
    
private:
    ProgressBar* m_pProgressBar{ nullptr };
    Widget *m_pSlider{ nullptr };
    float m_fMinValue{ 0.0f };
    float m_fMaxValue{ 100.0f };
    float m_fValue{ 0.0f };
    
    float sliderPosition() const noexcept;
    
    
};

}

}


#endif /* Slider_h */
