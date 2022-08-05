//
//  TerrainHighlightShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 26.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/TerrainHighlightShaderConfigurator.h>
#include <Interfaces/IShaderProgram.h>
#include <Renderer/RenderEnvironment.h>

namespace Ice {

void TerrainHighlightShaderConfigurator::setShaderProgram(IShaderProgram *p) noexcept {
    m_pShaderProgram = p;
}

void TerrainHighlightShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
}

void TerrainHighlightShaderConfigurator::loadUniforms(const RenderEnvironment &env) noexcept {
    m_pShaderProgram->loadMatrix4f(m_nProjectionMatrixUniformId, env.projectionMatrix);
    m_pShaderProgram->loadMatrix4f(m_nViewMatrixUniformId, env.viewMatrix);
}

void TerrainHighlightShaderConfigurator::getUniformLocations() noexcept {
    m_nProjectionMatrixUniformId = m_pShaderProgram->getUniformLocation("projectionMatrix");
    m_nViewMatrixUniformId = m_pShaderProgram->getUniformLocation("viewMatrix");
    m_nModelMatrixUniformId = m_pShaderProgram->getUniformLocation("modelMatrix");
}

void TerrainHighlightShaderConfigurator::loadModelMatrix(const glm::mat4& m) const noexcept {
    m_pShaderProgram->loadMatrix4f(m_nModelMatrixUniformId, m);
}

}
