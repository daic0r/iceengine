//
//  WidgetShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/WidgetShaderConfigurator.h>
#include <Interfaces/IShaderProgram.h>

namespace Ice {

const std::string_view WidgetShaderConfigurator::m_strPositionUniformName{ "position" };
const std::string_view WidgetShaderConfigurator::m_strScaleFactorsUniformName{ "scaleFactors" };
const std::string_view WidgetShaderConfigurator::m_strAlignOffsetUniformName{ "alignOffset" };
const std::string_view WidgetShaderConfigurator::m_strColorUniformName{ "color" };
const std::string_view WidgetShaderConfigurator::m_strForegroundColorUniformName{ "foregroundColor" };
const std::string_view WidgetShaderConfigurator::m_strMouseOverUniformName{ "isMouseOver" };
const std::string_view WidgetShaderConfigurator::m_strIsTexturedUniformName{ "isTextured" };
const std::string_view WidgetShaderConfigurator::m_strTextureUniformName{ "imageTexture" };
const std::string_view WidgetShaderConfigurator::m_strAspectRatioUniformName{ "aspectRatio" };

void WidgetShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
}

void WidgetShaderConfigurator::getUniformLocations() noexcept {
    UIElementShaderConfigurator::getUniformLocations();
    
    m_nPositionUniformId = m_pShaderProgram->getUniformLocation(m_strPositionUniformName.data());
    m_nScaleFactorsUniformId = m_pShaderProgram->getUniformLocation(m_strScaleFactorsUniformName.data());
    m_nAlignOffsetUniformId = m_pShaderProgram->getUniformLocation(m_strAlignOffsetUniformName.data());
    m_nColorUniformId = m_pShaderProgram->getUniformLocation(m_strColorUniformName.data());
    m_nForegroundColorUniformId = m_pShaderProgram->getUniformLocation(m_strForegroundColorUniformName.data());
    m_nMouseOverUniformId = m_pShaderProgram->getUniformLocation(m_strMouseOverUniformName.data());
    m_nIsTexturedUniformId = m_pShaderProgram->getUniformLocation(m_strIsTexturedUniformName.data());
    m_nTextureUniformId = m_pShaderProgram->getUniformLocation(m_strTextureUniformName.data());
    m_nAspectRatioUniformId = m_pShaderProgram->getUniformLocation(m_strAspectRatioUniformName.data());
    m_nBorderColorUniformId = m_pShaderProgram->getUniformLocation(m_strBorderColorUniformName.data());
    m_nBorderThicknessUniformId = m_pShaderProgram->getUniformLocation(m_strBorderThicknessUniformName.data());
    m_nCornerRadiusUniformId = m_pShaderProgram->getUniformLocation(m_strCornerRadiusUniformName.data());
    m_pShaderProgram->use();
    m_pShaderProgram->loadInt(m_nTextureUniformId, 0);
    m_pShaderProgram->unuse();
}

void WidgetShaderConfigurator::loadPosition(const glm::vec2 &pos) noexcept {
    m_pShaderProgram->loadVector2f(m_nPositionUniformId, pos);
}

void WidgetShaderConfigurator::loadScaleFactors(const glm::vec2 &scale) noexcept {
    m_pShaderProgram->loadVector2f(m_nScaleFactorsUniformId, scale);
}

void WidgetShaderConfigurator::loadAlignOffset(float x, float y) noexcept {
    m_pShaderProgram->loadVector2f(m_nAlignOffsetUniformId, glm::vec2{ x, y } );
}

void WidgetShaderConfigurator::loadColor(const glm::vec4 &color) noexcept {
    m_pShaderProgram->loadVector4f(m_nColorUniformId, color);
}

void WidgetShaderConfigurator::loadForegroundColor(const glm::vec4& color) noexcept {
    m_pShaderProgram->loadVector4f(m_nForegroundColorUniformId, color);
}

void WidgetShaderConfigurator::loadMouseOver(bool b) noexcept {
    m_pShaderProgram->loadInt(m_nMouseOverUniformId, b ? 1 : 0);
}

void WidgetShaderConfigurator::loadIsTextured(bool b) noexcept {
    m_pShaderProgram->loadInt(m_nIsTexturedUniformId, b ? 1 : 0);
}

void WidgetShaderConfigurator::loadAspectRatio(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nAspectRatioUniformId, f);
}

void WidgetShaderConfigurator::loadBorderColor(const glm::vec4& color) noexcept {
    m_pShaderProgram->loadVector4f(m_nBorderColorUniformId, color);
}

void WidgetShaderConfigurator::loadBorderThickness(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nBorderThicknessUniformId, f);
}

void WidgetShaderConfigurator::loadCornerRadius(float f) noexcept {
    m_pShaderProgram->loadFloat(m_nCornerRadiusUniformId, f);
}

}
