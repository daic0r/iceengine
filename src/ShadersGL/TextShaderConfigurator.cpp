//
//  TextShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 05.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/TextShaderConfigurator.h>
#include <Interfaces/IShaderProgram.h>

namespace Ice {

const std::string TextShaderConfigurator::m_strTextureUniformName{ "fontTexture" };
const std::string TextShaderConfigurator::m_strPositionUniformName{ "position" };
const std::string TextShaderConfigurator::m_strRotationUniformName{ "rotation" };
const std::string TextShaderConfigurator::m_strTextColorUniformName{ "textColor" };
const std::string TextShaderConfigurator::m_strBorderColorUniformName{ "borderColor" };
const std::string TextShaderConfigurator::m_strThicknessUniformName{ "thickness" };
const std::string TextShaderConfigurator::m_strEdgeWidthUniformName{ "edgeWidth" };
const std::string TextShaderConfigurator::m_strBorderWidthUniformName{ "borderWidth" };
const std::string TextShaderConfigurator::m_strBorderEdgeUniformName{ "borderEdge" };
const std::string TextShaderConfigurator::m_strShadowOffsetUniformName{ "shadowOffset" };
//uniform vec3 borderColor;
//uniform float thickness;
//uniform float edgeWidth;
//uniform float borderWidth;
//uniform float borderEdge;

void TextShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
    m_pShaderProgram->bindAttribute(1, "texCoord");
}

void TextShaderConfigurator::getUniformLocations() noexcept {
    UIElementShaderConfigurator::getUniformLocations();
    
    m_nTextureUniformId = m_pShaderProgram->getUniformLocation(m_strTextureUniformName);
    m_nPositionUniformId = m_pShaderProgram->getUniformLocation(m_strPositionUniformName);
    m_nRotationUniformId = m_pShaderProgram->getUniformLocation(m_strRotationUniformName);
    m_nTextColorUniformId = m_pShaderProgram->getUniformLocation(m_strTextColorUniformName);
    m_nBorderColorUniformId = m_pShaderProgram->getUniformLocation(m_strBorderColorUniformName);
    m_nThicknessUniformId = m_pShaderProgram->getUniformLocation(m_strThicknessUniformName);
    m_nEdgeWidthUniformId = m_pShaderProgram->getUniformLocation(m_strEdgeWidthUniformName);
    m_nBorderWidthUniformId  = m_pShaderProgram->getUniformLocation(m_strBorderWidthUniformName);
    m_nBorderEdgeUniformId = m_pShaderProgram->getUniformLocation(m_strBorderEdgeUniformName);
    m_nShadowOffsetUniformId = m_pShaderProgram->getUniformLocation(m_strShadowOffsetUniformName);
    m_pShaderProgram->use();
    m_pShaderProgram->loadInt(m_nTextureUniformId, 0);
    m_pShaderProgram->unuse();
}

void TextShaderConfigurator::loadTextColor(const glm::vec4& color) noexcept {
    m_pShaderProgram->loadVector4f(m_nTextColorUniformId, color);
}

void TextShaderConfigurator::loadBorderColor(const glm::vec4& color) noexcept {
    m_pShaderProgram->loadVector4f(m_nBorderColorUniformId, color);
}

void TextShaderConfigurator::loadThickness(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nThicknessUniformId, f);
}

void TextShaderConfigurator::loadEdgeWidth(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nEdgeWidthUniformId, f);
}

void TextShaderConfigurator::loadBorderWidth(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nBorderWidthUniformId, f);
}

void TextShaderConfigurator::loadBorderEdge(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nBorderEdgeUniformId, f);
}

void TextShaderConfigurator::loadShadowOffset(const glm::vec2& offset) noexcept {
    m_pShaderProgram->loadVector2f(m_nShadowOffsetUniformId, offset);
}

void TextShaderConfigurator::loadRotation(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nRotationUniformId, f);
}

void TextShaderConfigurator::loadPosition(const glm::vec2& pos) noexcept {
    m_pShaderProgram->loadVector2f(m_nPositionUniformId, pos);
}

}
