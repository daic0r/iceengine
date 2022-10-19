//
//  LightShader.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 06.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <glad/glad.h>
#include <ShadersGL/LightShaderConfigurator.h>
#include <Renderer/RenderEnvironment.h>
#include <System/Light.h>

namespace Ice {

const std::string LightShaderConfigurator::m_strLightPositionUniformName{ "lightPos" };
const std::string LightShaderConfigurator::m_strLightColorUniformName{ "lightColor" };
const std::string LightShaderConfigurator::m_strPlaneUniformName{ "plane" };

//void LightShader::bindAttributes() const noexcept {
//    Shader3d::bindAttributes();
//}

void LightShaderConfigurator::getUniformLocations() noexcept {
    Shader3dConfigurator::getUniformLocations();
    m_nLightPositionUniformId = m_pShaderProgram->getUniformLocation(m_strLightPositionUniformName);
    m_nLightColorUniformId = m_pShaderProgram->getUniformLocation(m_strLightColorUniformName);
    m_nPlaneUniformId = m_pShaderProgram->getUniformLocation(m_strPlaneUniformName);
}

void LightShaderConfigurator::loadLightPosition(const glm::vec3& pos) noexcept {
    if (m_nLightPositionUniformId != -1) {
        m_pShaderProgram->loadVector3f(m_nLightPositionUniformId, pos);
    }
}

void LightShaderConfigurator::loadLightColor(const glm::vec3& color) noexcept {
    if (m_nLightColorUniformId != -1) {
        m_pShaderProgram->loadVector3f(m_nLightColorUniformId, color);
    }
}

void LightShaderConfigurator::loadLightColor(float r, float g, float b) noexcept {
    loadLightColor(glm::vec3 { r, g, b });
}

void LightShaderConfigurator::loadPlane(const glm::vec4 &plane) noexcept {
    if (m_nPlaneUniformId != -1)
        m_pShaderProgram->loadVector4f(m_nPlaneUniformId, plane);
}

void LightShaderConfigurator::loadUniforms(const RenderEnvironment &env) noexcept {
    Shader3dConfigurator::loadUniforms(env);
    if (env.lights.size() > 0) {
        auto pLight = env.lights.at(0);
        loadLightPosition(pLight->position());
        loadLightColor(pLight->color());
    }
    loadPlane(env.clipPlane);
}

}
