//
//  UIElementShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/UIElementShaderConfigurator.h>
#include <Interfaces/IShaderProgram.h>

namespace Ice {

void UIElementShaderConfigurator::getUniformLocations() noexcept {
    m_nMinPointUniformId = m_pShaderProgram->getUniformLocation(m_strMinPointUniformName);
    m_nMaxPointUniformId = m_pShaderProgram->getUniformLocation(m_strMaxPointUniformName);
    m_nMinMaxTexCoordUniformId = m_pShaderProgram->getUniformLocation(m_strMinMaxTexCoorddUniformName);
}

void UIElementShaderConfigurator::loadMinPoint(const glm::vec2 &p) noexcept {
    m_pShaderProgram->loadVector2f(m_nMinPointUniformId, p);
}

void UIElementShaderConfigurator::loadMaxPoint(const glm::vec2 &p) noexcept {
    m_pShaderProgram->loadVector2f(m_nMaxPointUniformId, p);
}

void UIElementShaderConfigurator::loadMinMaxTexCoord(const glm::vec2& minPoint, const glm::vec2& maxPoint) noexcept {
    glm::vec4 minMaxPoint{ minPoint.x, minPoint.y, maxPoint.x, maxPoint.y };
    m_pShaderProgram->loadVector4f(m_nMinMaxTexCoordUniformId, minMaxPoint);
}

}
