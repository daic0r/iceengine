#include <ShadersGL/TexDisplayShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>

namespace Ice {

void TexDisplayShaderConfigurator::bindAttributes() const noexcept {
	m_pShaderProgram->bindAttribute(0, "vertexPos");
}

void TexDisplayShaderConfigurator::getUniformLocations() noexcept {
	m_nTexUniformId = m_pShaderProgram->getUniformLocation("tex");
	m_pShaderProgram->use();
	m_pShaderProgram->loadInt(m_nTexUniformId, 0);
	m_pShaderProgram->unuse();
}

void TexDisplayShaderConfigurator::loadUniforms(const RenderEnvironment&) noexcept {
}

}