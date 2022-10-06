//
//  TerrainShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glad.h>
#include <SDL2/SDL_opengl.h>
#include <ShadersGL/TerrainShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>

namespace Ice {

const std::string TerrainShaderConfigurator::m_strShadowMapTextureUniformName{ "shadowMap" };
const std::string TerrainShaderConfigurator::m_strWaterLevelClipPlaneYUniformName{ "waterLevelAndClipPlaneY" };

void TerrainShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
    m_pShaderProgram->bindAttribute(1, "color");
    m_pShaderProgram->bindAttribute(2, "normal");
}

void TerrainShaderConfigurator::getUniformLocations() noexcept {
	Shader3dConfigurator::getUniformLocations();
	m_nCommonMatricesUBOIndex = glGetUniformBlockIndex(m_pShaderProgram->id(), "CommonMatrices");
	glUniformBlockBinding(m_pShaderProgram->id(), m_nCommonMatricesUBOIndex, 0);
	//m_nShadowProjViewMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strShadowProjViewMatrixUniformName);
	m_nShadowMapTextureUniformId = m_pShaderProgram->getUniformLocation(m_strShadowMapTextureUniformName);
	m_nWaterLevelClipPlaneYUniformId = m_pShaderProgram->getUniformLocation(m_strWaterLevelClipPlaneYUniformName);
	//// Use texture unit 1
	m_pShaderProgram->use();
	m_pShaderProgram->loadInt(m_nShadowMapTextureUniformId, 1);
	m_pShaderProgram->unuse();
	//m_nShadowDistanceUniformId = m_pShaderProgram->getUniformLocation(m_strShadowDistanceUniformName);
	//m_nShadowMarginUniformId = m_pShaderProgram->getUniformLocation(m_strShadowMarginUniformName);
}

void TerrainShaderConfigurator::loadWaterLevelAndClipPlaneY(float fWaterLevel, int nClipPlaneY) noexcept {
	m_pShaderProgram->loadVector2f(m_nWaterLevelClipPlaneYUniformId, glm::vec2{ fWaterLevel, nClipPlaneY });
}

//void TerrainShaderConfigurator::loadShadowProjViewMatrix(const glm::mat4& m) const noexcept {
//	if (m_nShadowProjViewMatrixUniformId != -1)
//		m_pShaderProgram->loadMatrix4f(m_nShadowProjViewMatrixUniformId, m);
//}
//
//// Distance from the camera within which shadows will be taken into account
//void TerrainShaderConfigurator::loadShadowDistance(float f) const noexcept {
//	if (m_nShadowDistanceUniformId != -1)
//		m_pShaderProgram->loadFloat(m_nShadowDistanceUniformId, f);
//}
//
//// Transition margin before the end of the shadow box
//void TerrainShaderConfigurator::loadShadowMargin(float m) const noexcept {
//	if (m_nShadowMarginUniformId != -1)
//		m_pShaderProgram->loadFloat(m_nShadowMarginUniformId, m);
//}

}
