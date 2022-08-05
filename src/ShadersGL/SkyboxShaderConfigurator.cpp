//
//  SkyboxShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 05.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/SkyboxShaderConfigurator.h>
#include <Renderer/RenderEnvironment.h>

namespace Ice {

const std::string SkyboxShaderConfigurator::m_strRotationUniformName = "rotation";

void SkyboxShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
    m_pShaderProgram->bindAttribute(1, "texCoord");
}

void SkyboxShaderConfigurator::getUniformLocations() noexcept {
	Shader3dConfigurator::getUniformLocations();
	m_nRotationUniformId = m_pShaderProgram->getUniformLocation(m_strRotationUniformName);
}

void SkyboxShaderConfigurator::loadUniforms(const RenderEnvironment &env) noexcept {
    loadProjectionMatrix(env.projectionMatrix);
    auto viewMatrix = env.viewMatrix;
    viewMatrix[3][0] = 0.0f;
    viewMatrix[3][1] = 0.0f;
    viewMatrix[3][2] = 0.0f;
    loadViewMatrix(viewMatrix);
}

void SkyboxShaderConfigurator::loadRotation(float fAngle) noexcept {
	m_pShaderProgram->loadFloat(m_nRotationUniformId, fAngle);
}

}
