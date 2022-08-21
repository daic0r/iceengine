//
//  MousePicker.h
//  IceCraft
//
//  Created by Matthias Gruen on 26.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MousePicker_h
#define MousePicker_h

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace Ice {

class IGraphicsSystem;

class MousePicker {
    
    IGraphicsSystem *m_pGraphicsSystem{ nullptr };
    int m_nMouseX{}, m_nMouseY{};
    glm::mat4 m_invView, m_invProjection;
    
public:
    MousePicker(int mouseX, int mouseY, const glm::mat4&) noexcept;
    
    glm::vec3 getMouseRay() const noexcept;
};

}

#endif /* MousePicker_h */
