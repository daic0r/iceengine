#include <GL/glew.h>
#include <Renderer/OpenGL/UniformBufferObjectGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <utility>


namespace Ice {

UniformBufferObjectGL::UniformBufferObjectGL(GLuint nBindingPointIndex, std::string_view strBlockName, size_t nSize, Layout l) 
	: m_nBindingPointIndex{ nBindingPointIndex }, m_strBlockName { strBlockName }, m_nBufSize{ nSize }, m_layout{ l }
{
	allocateBuffer(nSize);
	glCall(glBindBufferRange(GL_UNIFORM_BUFFER, nBindingPointIndex, m_nUBO, 0, nSize));
}

UniformBufferObjectGL::UniformBufferObjectGL(UniformBufferObjectGL&& other) noexcept {
	m_nUBO = std::exchange(other.m_nUBO, 0);
	m_nBindingPointIndex = std::exchange(other.m_nBindingPointIndex, 0);
	m_strBlockName = std::exchange(other.m_strBlockName, "");
	m_nBufSize = std::exchange(other.m_nBufSize, 0);
	m_layout = std::exchange(other.m_layout, Layout::std140);
}

UniformBufferObjectGL& UniformBufferObjectGL::operator=(UniformBufferObjectGL&& other) noexcept {
	UniformBufferObjectGL copy{ std::move(other) };
	swap(copy);
	return *this;
}

UniformBufferObjectGL::~UniformBufferObjectGL() {
	if (m_nUBO != 0) {
		glCall(glDeleteBuffers(1, &m_nUBO));
	}
}

void UniformBufferObjectGL::allocateBuffer(size_t nSize) noexcept {
	glCall(glGenBuffers(1, &m_nUBO));
	glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_nUBO));
	glCall(glBufferData(GL_UNIFORM_BUFFER, nSize, nullptr, GL_STREAM_DRAW));
	glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBufferObjectGL::loadMatrix(size_t nOffset, const glm::mat4& m) const noexcept {
	glCall(glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(glm::mat4), glm::value_ptr(m)));
}

void UniformBufferObjectGL::loadFloat(size_t nOffset, float f) const noexcept {
	glCall(glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(float), &f));
}

void UniformBufferObjectGL::loadInt(size_t nOffset, int i) const noexcept {
	glCall(glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(int), &i));
}

void UniformBufferObjectGL::loadVector3(size_t nOffset, const glm::vec3& v) const noexcept {
	glCall(glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(glm::vec3), &v));
}

void UniformBufferObjectGL::loadVector4(size_t nOffset, const glm::vec4& v) const noexcept {
	glCall(glBufferSubData(GL_UNIFORM_BUFFER, nOffset, sizeof(glm::vec4), &v));
}

void UniformBufferObjectGL::swap(UniformBufferObjectGL& other) noexcept {
	using std::swap;
	swap(m_nUBO, other.m_nUBO);
	swap(m_nBindingPointIndex, other.m_nBindingPointIndex);
	swap(m_strBlockName, other.m_strBlockName);
	swap(m_nBufSize, other.m_nBufSize);
	swap(m_layout, other.m_layout);
}

void UniformBufferObjectGL::bind() const noexcept {
	glCall(glBindBuffer(GL_UNIFORM_BUFFER, m_nUBO));
}

void UniformBufferObjectGL::unbind() const noexcept {
	glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

}