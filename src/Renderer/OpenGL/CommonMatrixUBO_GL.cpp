#include <Renderer/OpenGL/CommonMatrixUBO_GL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ice {

CommonMatrixUBO_GL::CommonMatrixUBO_GL() :
	UniformBufferObjectGL{ 0, "CommonMatrices", sizeof(glm::mat4) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(glm::vec4) + sizeof(glm::vec4) + sizeof(glm::vec4) }
{
}

void CommonMatrixUBO_GL::loadShadowProjViewMatrix(const glm::mat4& m) const noexcept {
	loadMatrix(m_arUniforms[0].second, m);
}

void CommonMatrixUBO_GL::loadShadowDistance(float f) const noexcept {
	loadFloat(m_arUniforms[1].second, f);
}

void CommonMatrixUBO_GL::loadShadowMargin(float f) const noexcept {
	loadFloat(m_arUniforms[2].second, f);
}

void CommonMatrixUBO_GL::loadShadowTexelSize(float f) const noexcept {
	loadFloat(m_arUniforms[3].second, f);
}

void CommonMatrixUBO_GL::loadSunPosition(const glm::vec3& p) const noexcept {
	loadVector4(m_arUniforms[4].second, glm::vec4{ p, 1.0f });
}

void CommonMatrixUBO_GL::loadSunColor(const glm::vec3& c) const noexcept {
	loadVector4(m_arUniforms[5].second, glm::vec4{ c, 1.0f });
}

void CommonMatrixUBO_GL::loadSunAmbient(const glm::vec3& c) const noexcept {
	loadVector4(m_arUniforms[6].second, glm::vec4{ c, 1.0f });
}

}