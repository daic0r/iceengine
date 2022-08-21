//
//  MousePicker.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 26.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/MousePicker.h>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

MousePicker::MousePicker(int mouseX, int mouseY, const glm::mat4& viewMatrix) noexcept
    : m_nMouseX{ mouseX },
    m_nMouseY{ mouseY },
    m_invView{ glm::inverse(viewMatrix) },
    m_pGraphicsSystem{ systemServices.getGraphicsSystem() },
    m_invProjection{ glm::inverse(m_pGraphicsSystem->projectionMatrix()) }
{
}

glm::vec3 MousePicker::getMouseRay() const noexcept {
    
    const float fDisplayWidth = m_pGraphicsSystem->displayWidth();
    const float fDisplayHeight = m_pGraphicsSystem->displayHeight();
    
    // To homogeneous clip space
    float fX = 2.0f * (static_cast<float>(m_nMouseX) / fDisplayWidth) - 1.0f;
    float fY = 1.0f - (2.0f * (static_cast<float>(m_nMouseY) / fDisplayHeight));
    glm::vec4 worldSpace{ fX, fY, -1.0f, 1.0f };
    
    // To eye space
    worldSpace = m_invProjection * worldSpace;
    worldSpace.z = -1.0f;
    worldSpace.w = 0.0f;
    
    // To world space
    worldSpace = m_invView * worldSpace;
    
    glm::vec3 ret{ worldSpace.x, worldSpace.y, worldSpace.z };
    ret = glm::normalize(ret);
    
    return ret;
}

}
