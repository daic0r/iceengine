//
//  AnimatedModelShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 18.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glad.h>
#include <ShadersGL/AnimatedModelShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <ModelAnimation/Joint.h>
#include <algorithm>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <Renderer/OpenGL/RenderToolsGL.h>

namespace Ice {

AnimatedModelShaderConfigurator::~AnimatedModelShaderConfigurator() {
	if (m_nTexObjDataBuffer != 0) {
		glCall(glDeleteBuffers(1, &m_nTexObjDataBuffer));
	}
	if (m_nTexObj != 0) {
		glCall(glDeleteTextures(1, &m_nTexObj));
	}
}

void AnimatedModelShaderConfigurator::bindAttributes() const noexcept {
    ModelShaderConfigurator::bindAttributes();
    m_pShaderProgram->bindAttribute(7, "jointIds");	// 3,4,5,6 taken up by the 4 columns of the world transform matrix
    m_pShaderProgram->bindAttribute(8, "jointWeights");
}

void AnimatedModelShaderConfigurator::getUniformLocations() noexcept {
    ModelShaderConfigurator::getUniformLocations();
	
	m_pShaderProgram->use();
	m_nTexObjUniformID = m_pShaderProgram->getUniformLocation("jointTransforms_TBO");
	glCall(glUniform1i(m_nTexObjUniformID, 1)); // texture unit #1
	m_pShaderProgram->unuse();
	
	glCall(glGenBuffers(1, &m_nTexObjDataBuffer));
	glCall(glBindBuffer(GL_TEXTURE_BUFFER, m_nTexObjDataBuffer));
	glCall(glBufferData(GL_TEXTURE_BUFFER, MAX_INSTANCES * MAX_JOINTS * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW));

	glCall(glCreateTextures(GL_TEXTURE_BUFFER, 1, &m_nTexObj));
	glCall(glTextureBuffer(m_nTexObj, GL_RGBA32F, m_nTexObjDataBuffer));
	glCall(glBindBuffer(GL_TEXTURE_BUFFER, 0));
    //for (size_t i = 0; i < kMAX_JOINTS; ++i) {
    //    m_nJointTransformUniformIds[i] = m_pShaderProgram->getUniformLocation("jointTransforms[" + std::to_string(i) + "]");
    //}
}

void AnimatedModelShaderConfigurator::loadJointTransforms(const std::vector<glm::mat4>& vJointTransforms) noexcept {
  //  for (size_t i = 0; i < std::min(vJointTransforms.size(), MAX_JOINTS); ++i) {
		//m_pShaderProgram->loadMatrix4f(m_nJointTransformsUniformIds[i], vJointTransforms[i]);
  //  }

	glCall(glBindBuffer(GL_TEXTURE_BUFFER, m_nTexObjDataBuffer));
	glCall(glBufferSubData(GL_TEXTURE_BUFFER, 0, vJointTransforms.size() * sizeof(glm::mat4), &vJointTransforms[0]));
	glCall(glBindBuffer(GL_TEXTURE_BUFFER, 0));

	glCall(glActiveTexture(GL_TEXTURE0 + 1));
	glCall(glBindTexture(GL_TEXTURE_BUFFER, m_nTexObj));

}

}
