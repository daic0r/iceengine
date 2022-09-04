//
//  ModelShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 25.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <ShadersGL/ModelShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <iostream>
#include <Renderer/RenderEnvironment.h>

namespace Ice {

const std::string ModelShaderConfigurator::m_strProjMatrixUniformName{ "projection" };
const std::string ModelShaderConfigurator::m_strViewMatrixUniformName{ "view" };
//const std::string ModelShaderConfigurator::m_strModelMatrixUniformName{ "model" };
const std::string ModelShaderConfigurator::m_strTextureUniformName{ "diffuseTex" };

const std::string ModelShaderConfigurator::m_strMaterialAmbientUniformName{ "ambient" };
const std::string ModelShaderConfigurator::m_strMaterialDiffuseUniformName{ "diffuse" };
const std::string ModelShaderConfigurator::m_strMaterialSpecularUniformName{ "specular" };
const std::string ModelShaderConfigurator::m_strMaterialSpecularExponentUniformName{ "fSpecularExponent" };
const std::string ModelShaderConfigurator::m_strMaterialAlphaUniformName{ "fAlpha" };
const std::string ModelShaderConfigurator::m_strMaterialRefractionIndexUniformName{ "fRefractionIndex" };
const std::string ModelShaderConfigurator::m_strMaterialHasTextureUniformName{ "hasTexture" };

const std::string ModelShaderConfigurator::m_strShadowProjViewMatrixUniformName{ "shadowProjViewMatrix" };
const std::string ModelShaderConfigurator::m_strShadowMapTextureUniformName{ "shadowMap" };
const std::string ModelShaderConfigurator::m_strShadowDistanceUniformName{ "shadowDistance" };
const std::string ModelShaderConfigurator::m_strShadowMarginUniformName{ "shadowMargin" };

const std::string ModelShaderConfigurator::m_strWaterLevelClipPlaneYUniformName{ "waterLevelAndClipPlaneY" };

//sunPosition
// sunColor
//const std::vector<const GLchar*> ModelShaderConfigurator::MaterialUBOVarNames {
//   "ambient", "diffuse", "specular", "fSpecularExponent", "fAlpha", "fRefractionIndex", "hasTexture"
//};

ModelShaderConfigurator::~ModelShaderConfigurator() {
//    if (m_nMaterialUBO != 0)
//        glCall(glDeleteBuffers(1, &m_nMaterialUBO));
}

void ModelShaderConfigurator::setShaderProgram(IShaderProgram *pProgram) noexcept {
    m_pShaderProgram = dynamic_cast<ShaderProgramGL*>(pProgram);
}

void ModelShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
    m_pShaderProgram->bindAttribute(1, "texCoord");
    m_pShaderProgram->bindAttribute(2, "normal");
	m_pShaderProgram->bindAttribute(3, "worldTransform");
	//m_pShaderProgram->bindAttribute(4, "worldTransCol2");
	//m_pShaderProgram->bindAttribute(5, "worldTransCol3");
	//m_pShaderProgram->bindAttribute(6, "worldTransCol4");
}

void ModelShaderConfigurator::initialize() noexcept {
    

}

void ModelShaderConfigurator::loadUniforms(const RenderEnvironment& env) noexcept {
    loadViewMatrix(env.viewMatrix);
    loadProjectionMatrix(env.projectionMatrix);
}

void ModelShaderConfigurator::getUniformLocations() noexcept {
    m_nProjectMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strProjMatrixUniformName);
    m_nViewMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strViewMatrixUniformName);
    //m_nModelMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strModelMatrixUniformName);
    m_nTextureUniformId = m_pShaderProgram->getUniformLocation(m_strTextureUniformName);
    
    m_nMaterialAmbientUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialAmbientUniformName);
    m_nMaterialDiffuseUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialDiffuseUniformName);
    m_nMaterialSpecularUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialSpecularUniformName);
    m_nMaterialSpecularExponentUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialSpecularExponentUniformName);
    m_nMaterialAlphaUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialAlphaUniformName);
    m_nMaterialRefractionIndexUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialRefractionIndexUniformName);
    m_nMaterialHasTextureUniformId = m_pShaderProgram->getUniformLocation(m_strMaterialHasTextureUniformName);
	m_nWaterLevelClipPlaneYUniformId = m_pShaderProgram->getUniformLocation(m_strWaterLevelClipPlaneYUniformName);

	m_nCommonMatricesUBOIndex = glGetUniformBlockIndex(m_pShaderProgram->id(), "CommonMatrices");
	glCall(glUniformBlockBinding(m_pShaderProgram->id(), m_nCommonMatricesUBOIndex, 0));
	//m_nShadowProjViewMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strShadowProjViewMatrixUniformName);
	m_nShadowMapTextureUniformId = m_pShaderProgram->getUniformLocation(m_strShadowMapTextureUniformName);

    m_pShaderProgram->use();
	// Use texture unit 1
	if (m_nShadowMapTextureUniformId != 1) {
		m_pShaderProgram->loadInt(m_nShadowMapTextureUniformId, 1);
	}
    loadWaterLevelAndClipPlaneY(0.0f, 0);
    m_pShaderProgram->unuse();
	//m_nShadowDistanceUniformId = m_pShaderProgram->getUniformLocation(m_strShadowDistanceUniformName);
	//m_nShadowMarginUniformId = m_pShaderProgram->getUniformLocation(m_strShadowMarginUniformName);

	
//    m_pMaterialUBOBuffer = RenderToolsGL::createUniformBuffer(m_pShaderProgram->id(), "Material", MaterialUBOVarNames, m_vMaterialUBOVarOffsets, m_nMaterialUBO, m_nMaterialUBOBufferSize);
    
//    m_nMaterialUBOIndex = glCall(glGetUniformBlockIndex(m_pShaderProgram->id(), "Material"));
//    glCall(glGetActiveUniformBlockiv(m_pShaderProgram->id(), m_nMaterialUBOIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &m_nMaterialUBOBufSize));
//    m_pMaterialUBOBuffer = std::make_unique<unsigned char[]>(m_nMaterialUBOBufSize);
//
//    glCall(glGetUniformIndices(m_pShaderProgram->id(), 6, MaterialUBOVarNames, m_arMaterialUBOVarIndices));
//    glCall(glGetActiveUniformsiv(m_pShaderProgram->id(), 6, m_arMaterialUBOVarIndices, GL_UNIFORM_OFFSET, m_arMaterialUBOVarOffsets));
//
//    glCall(glGenBuffers(1, &m_nMaterialUBO));
//    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_nMaterialUBO));
//    glCall(glBufferData(GL_UNIFORM_BUFFER, m_nMaterialUBOBufSize, nullptr, GL_DYNAMIC_DRAW));
//    glCall(glBindBufferBase(GL_UNIFORM_BUFFER, m_nMaterialUBOIndex, m_nMaterialUBO));
//    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void ModelShaderConfigurator::setRenderMaterial(const RenderMaterial& mat) noexcept {
    m_material = mat;
    m_pShaderProgram->loadVector3f(m_nMaterialAmbientUniformId, m_material.ambient());
    m_pShaderProgram->loadVector3f(m_nMaterialDiffuseUniformId, m_material.diffuse());
    m_pShaderProgram->loadVector3f(m_nMaterialSpecularUniformId, m_material.specular());
    m_pShaderProgram->loadFloat(m_nMaterialSpecularExponentUniformId, m_material.specularExponent());
    m_pShaderProgram->loadFloat(m_nMaterialAlphaUniformId, m_material.alpha());
    m_pShaderProgram->loadFloat(m_nMaterialRefractionIndexUniformId, m_material.refractionIndex());
    m_pShaderProgram->loadInt(m_nMaterialHasTextureUniformId, !m_material.textureFile().empty() ? 1 : 0);

//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[0], &m_material.ambient(), sizeof(glm::vec3));
//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[1], &m_material.diffuse(), sizeof(glm::vec3));
//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[2], &m_material.specular(), sizeof(glm::vec3));
//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[3], &m_material.specularExponent(), sizeof(float));
//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[4], &m_material.alpha(), sizeof(float));
//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[5], &m_material.refractionIndex(), sizeof(float));
//    GLint bHasTexture = !m_material.textureFile().empty() ? 1 : 0;
//    memcpy(m_pMaterialUBOBuffer.get() + m_vMaterialUBOVarOffsets[6], &bHasTexture, sizeof(GLint));
//    glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_nMaterialUBO));
//    glCall(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_nMaterialUBOBufferSize, m_pMaterialUBOBuffer.get()));
//    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void ModelShaderConfigurator::loadProjectionMatrix(const glm::mat4& mat) const noexcept {
    if (m_nProjectMatrixUniformId != -1)
        m_pShaderProgram->loadMatrix4f(m_nProjectMatrixUniformId, mat);
}

void ModelShaderConfigurator::loadViewMatrix(const glm::mat4& mat) const noexcept {
    if (m_nViewMatrixUniformId != -1)
        m_pShaderProgram->loadMatrix4f(m_nViewMatrixUniformId, mat);
}

//void ModelShaderConfigurator::loadModelMatrix(const glm::mat4& mat) const noexcept {
//    if (m_nModelMatrixUniformId != -1)
//        m_pShaderProgram->loadMatrix4f(m_nModelMatrixUniformId, mat);
//}

void ModelShaderConfigurator::loadTextureUnit(GLuint nUnit) const noexcept {
    if (m_nTextureUniformId != -1) {
        m_pShaderProgram->loadInt(m_nTextureUniformId, nUnit);
    }
}

void ModelShaderConfigurator::loadWaterLevelAndClipPlaneY(float fWaterLevel, int nClipPlaneY) noexcept {
	m_pShaderProgram->loadVector2f(m_nWaterLevelClipPlaneYUniformId, glm::vec2{ fWaterLevel, nClipPlaneY });
}

/*
void ModelShaderConfigurator::loadSunPosition(const glm::vec3& pos) const noexcept {
	if (m_nSunPositionUniformId != -1) {
		m_pShaderProgram->loadVector3f(m_nSunPositionUniformId, pos);
	}
}

void ModelShaderConfigurator::loadSunColor(const glm::vec3& col) const noexcept {
	if (m_nSunColorUniformId != -1) {
		m_pShaderProgram->loadVector3f(m_nSunColorUniformId, col);
	}
}

void ModelShaderConfigurator::loadSunAmbient(const glm::vec3& col) const noexcept {
	if (m_nSunAmbientUniformId != -1) {
		m_pShaderProgram->loadVector3f(m_nSunAmbientUniformId, col);
	}
}
*/

//void ModelShaderConfigurator::loadShadowProjViewMatrix(const glm::mat4& m) const noexcept {
//	if (m_nShadowProjViewMatrixUniformId != -1)
//		m_pShaderProgram->loadMatrix4f(m_nShadowProjViewMatrixUniformId, m);
//}
//
//// Distance from the camera within which shadows will be taken into account
//void ModelShaderConfigurator::loadShadowDistance(float f) const noexcept {
//	if (m_nShadowDistanceUniformId != -1)
//		m_pShaderProgram->loadFloat(m_nShadowDistanceUniformId, f);
//}
//
//// Transition margin before the end of the shadow box
//void ModelShaderConfigurator::loadShadowMargin(float m) const noexcept {
//	if (m_nShadowMarginUniformId != -1)
//		m_pShaderProgram->loadFloat(m_nShadowMarginUniformId, m);
//}

}
