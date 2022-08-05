//
//  Button.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Button.h>
#include <Interfaces/IWidgetRenderer.h>
#include <iostream>


namespace Ice {

namespace GUI {

Button::Button(const std::string& strLabel) : m_strLabel{ strLabel }
{
    setIsTabStoppable(true);

    setOnMouseOverFunc([](Widget* inst, float x, float y, float offsetX, float offsetY, std::uint8_t buttonMask, bool& bHandled) {
        //std::cout << "Mouse at " << x << ", " << y << " ; " << "buttons: " << (int)buttonMask << std::endl;
    });
        
    setOnMouseEnterFunc([](Widget *w, bool& bHandled) {
        //std::cout << "Mouse entered" << std::endl;
        Button* b = static_cast<Button*>(w);
        b->m_oldColor = w->color();
        b->setColor(glm::vec4{ 0.0f, 0.0f, 8.0f, 0.8f });
    });

    setOnMouseLeaveFunc([](Widget* w, bool& bHandled) {
        //std::cout << "Mouse left" << std::endl;
        Button* b = static_cast<Button*>(w);
        b->setColor(b->m_oldColor);
    });
    
    setOnMouseButtonFunc([](Widget* w, MouseButton mb, bool bPressed, bool& bHandled) {
        if (!bPressed)
            std::cout << "Button clicked" << std::endl;
    });
}

void Button::render(IWidgetRenderer *pRenderer) {
    pRenderer->renderWidget(this);
    setGraphicsDirty(false);
}


}

}
