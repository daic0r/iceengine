#include <ShadersGL/SunShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <Renderer/RenderEnvironment.h>
#include <glm/vec3.hpp>

namespace Ice {

void SunShaderConfigurator::setShaderProgram(IShaderProgram *p) noexcept {
    m_pShaderProgram = p;
}

void SunShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
}

void SunShaderConfigurator::loadUniforms(const RenderEnvironment &env) noexcept {
    m_pShaderProgram->loadMatrix4f(m_nProjectionMatrixUniformId, env.projectionMatrix);
    m_pShaderProgram->loadMatrix4f(m_nViewMatrixUniformId, env.viewMatrix);
}

void SunShaderConfigurator::getUniformLocations() noexcept {
    m_nProjectionMatrixUniformId = m_pShaderProgram->getUniformLocation("projection");
    m_nViewMatrixUniformId = m_pShaderProgram->getUniformLocation("view");
    m_nPositionUniformId = m_pShaderProgram->getUniformLocation("sunPosition");
	m_nSunColorUniformId = m_pShaderProgram->getUniformLocation("sunColor");
	m_nViewportUniformId = m_pShaderProgram->getUniformLocation("viewport");
}

void SunShaderConfigurator::loadPosition(const glm::vec3& v) const noexcept {
	if (m_nPositionUniformId != -1) {
		m_pShaderProgram->loadVector3f(m_nPositionUniformId, v);
	}
}

void SunShaderConfigurator::loadSunColor(const glm::vec3& col) const noexcept {
	if (m_nSunColorUniformId != -1) {
		m_pShaderProgram->loadVector3f(m_nSunColorUniformId, col);
	}
}

void SunShaderConfigurator::loadViewportDimensions(int width, int height) const noexcept {
	if (m_nViewportUniformId != -1) {
		m_pShaderProgram->loadVector2f(m_nViewportUniformId, glm::vec2{ width, height });
	}
}

}