#include <glad/glad.h>
#include <ShadersGL/ShadowShaderConfigurator.h>
#include <Interfaces/IShaderProgram.h>
#include <Renderer/RenderEnvironment.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>

namespace Ice {

void ShadowShaderConfigurator::bindAttributes() const noexcept {
	m_pShaderProgram->bindAttribute(0, "vertexPos");
	m_pShaderProgram->bindAttribute(3, "worldTransform");
}

void ShadowShaderConfigurator::getUniformLocations() noexcept {
	m_nCommonMatricesUBOIndex = glGetUniformBlockIndex(m_pShaderProgram->id(), "CommonMatrices");
	glCall(glUniformBlockBinding(m_pShaderProgram->id(), m_nCommonMatricesUBOIndex, 0));
}

}