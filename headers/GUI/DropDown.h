//
//  DropDown.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef DropDown_h
#define DropDown_h

#include <GUI/Widget.h>
#include <System/Pixels.h>
#include <glm/vec4.hpp>

#include <unordered_map>

namespace Ice {

namespace GUI {

template<OrthogonalAxis>
class StackPanel;
class Button;
class WidgetView;

class DropDown : public Widget {
    Pixels m_buttonSize{ 25 };
    StackPanel<OrthogonalAxis::VERTICAL>* m_pStackPanel{ nullptr };
    Button* m_pDropDownButton{ nullptr };
    Widget* m_pSelected{ nullptr };
    WidgetView* m_pView{ nullptr };
    bool m_bInitialized{ false };
    std::unordered_map<Widget*, glm::vec4> m_mOldColors;
    
public:
    DropDown();
    
    auto buttonSize() const noexcept { return m_buttonSize; }
    auto dropDownButton() const noexcept { return m_pDropDownButton; }
    auto stackPanel() const noexcept { return m_pStackPanel; }
    auto view() const noexcept { return m_pView; }
    
    void addChild(Widget* w, size_t nDrawOrder) override;
};

}

}


#endif /* DropDown_h */
