#pragma once

#include <string>
#include <string_view>
#include <SDL2/SDL_opengl.h>
#include <glm/mat4x4.hpp>

namespace Ice {

class UniformBufferObjectGL {
public:
	enum class Layout {
		std140
	};

private:

	GLuint m_nUBO{ 0 };
	GLuint m_nBindingPointIndex{ 0 };
	std::string m_strBlockName;
	size_t m_nBufSize{ 0 };
	Layout m_layout{ Layout::std140 };

protected:

	UniformBufferObjectGL(GLuint nBindingPointIndex, std::string_view strBlockName, size_t nSize, Layout l = Layout::std140);
	UniformBufferObjectGL(const UniformBufferObjectGL&) = delete;
	UniformBufferObjectGL& operator=(const UniformBufferObjectGL&) = delete;
	UniformBufferObjectGL(UniformBufferObjectGL&&) noexcept;
	UniformBufferObjectGL& operator=(UniformBufferObjectGL&&) noexcept;
	virtual ~UniformBufferObjectGL();

	void allocateBuffer(size_t nSize) noexcept;
	void loadMatrix(size_t nOffset, const glm::mat4& m) const noexcept;
	void loadFloat(size_t nOffset, float) const noexcept;
	void loadInt(size_t nOffset, int) const noexcept;
	void loadVector3(size_t nOffset, const glm::vec3&) const noexcept;
	void loadVector4(size_t nOffset, const glm::vec4&) const noexcept;
	void swap(UniformBufferObjectGL& other) noexcept;
	friend void swap(UniformBufferObjectGL& lhs, UniformBufferObjectGL& rhs) noexcept {
		lhs.swap(rhs);
	}

public:

	void bind() const noexcept;
	void unbind() const noexcept;
	constexpr size_t bufSize() const noexcept { return m_nBufSize; }
};

}