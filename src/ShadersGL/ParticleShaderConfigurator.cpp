//
//  ParticleShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/ParticleShaderConfigurator.h>
#include <Renderer/RenderEnvironment.h>

namespace Ice {

const std::string ParticleShaderConfigurator::m_strProjectionMatrixUniformName{"projectionMatrix"};
const std::string ParticleShaderConfigurator::m_strParticleTextureUniformName{ "particleTexture" };
const std::string ParticleShaderConfigurator::m_strCellDimsUniformName{"cellDims"};
const std::string ParticleShaderConfigurator::m_strViewMatrixUniformName{"viewMatrix"};

void ParticleShaderConfigurator::bindAttributes() const noexcept {
//    m_pShaderProgram->bindAttribute(0, "vertexPos");
//    m_pShaderProgram->bindAttribute(1, "vertTranslation");
//    m_pShaderProgram->bindAttribute(2, "vertZRotation");
//    m_pShaderProgram->bindAttribute(3, "vertScale");
//    m_pShaderProgram->bindAttribute(4, "texOffsets");
//    m_pShaderProgram->bindAttribute(5, "blendFactor");
}

void ParticleShaderConfigurator::loadUniforms(const RenderEnvironment& env) noexcept {
}

void ParticleShaderConfigurator::getUniformLocations() noexcept {
    m_nProjectionMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strProjectionMatrixUniformName);
    m_nParticleTextureUniformId = m_pShaderProgram->getUniformLocation(m_strParticleTextureUniformName);
    m_nCellDimsUniformId = m_pShaderProgram->getUniformLocation(m_strCellDimsUniformName);
    m_nViewMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strViewMatrixUniformName);
    //m_nViewMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strViewMatrixUniformName);
}

void ParticleShaderConfigurator::loadProjectionMatrix(const glm::mat4 &proj) noexcept{
    m_pShaderProgram->loadMatrix4f(m_nProjectionMatrixUniformId, proj);
}

void ParticleShaderConfigurator::loadCellDims(float x, float y) noexcept {
    m_pShaderProgram->loadVector2f(m_nCellDimsUniformId, glm::vec2{ x, y });
}

void ParticleShaderConfigurator::loadViewMatrix(const glm::mat4& view) noexcept {
    m_pShaderProgram->loadMatrix4f(m_nViewMatrixUniformId, view);
}

}
