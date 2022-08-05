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
    
    glm::mat4 m_invView, m_invProjection;
    IGraphicsSystem *m_pGraphicsSystem{ nullptr };
    
public:
    MousePicker(const glm::mat4&) noexcept;
    
    glm::vec3 getMouseRay() const noexcept;
};

}

#endif /* MousePicker_h */
