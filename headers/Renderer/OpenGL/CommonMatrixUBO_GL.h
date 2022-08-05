#pragma once

#include <Renderer/OpenGL/UniformBufferObjectGL.h>
#include <array>
#include <string>
#include <glm/mat4x4.hpp>

namespace Ice {

class CommonMatrixUBO_GL : public UniformBufferObjectGL {

	static constexpr std::array<std::pair<const char*, std::size_t>, 7> m_arUniforms =
	{
		std::pair{ "shadowProjViewMatrix", 0 },
		std::pair{ "shadowDistance", sizeof(glm::mat4) },
		std::pair{ "shadowMargin", sizeof(glm::mat4) + sizeof(float) },
		std::pair{ "shadowTexelSize", sizeof(glm::mat4) + sizeof(float) + sizeof(float) },
		std::pair{ "sunPosition", sizeof(glm::mat4) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) }, // <-- extra float here for correct padding
		std::pair{ "sunColor", sizeof(glm::mat4) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(glm::vec4) },
		std::pair{ "sunAmbient", sizeof(glm::mat4) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(float) + sizeof(glm::vec4) + sizeof(glm::vec4) }
	};

public:
	CommonMatrixUBO_GL();
	//CommonMatrixUBO_GL(const CommonMatrixUBO_GL&) = delete;
	//CommonMatrixUBO_GL& operator=(const CommonMatrixUBO_GL&) = delete;
	//CommonMatrixUBO_GL(CommonMatrixUBO_GL&&) noexcept;
	//CommonMatrixUBO_GL& operator=(CommonMatrixUBO_GL&&) noexcept;

	void loadShadowProjViewMatrix(const glm::mat4&) const noexcept;
	void loadShadowDistance(float) const noexcept;
	void loadShadowMargin(float) const noexcept;
	void loadShadowTexelSize(float) const noexcept;
	void loadSunPosition(const glm::vec3&) const noexcept;
	void loadSunColor(const glm::vec3&) const noexcept;
	void loadSunAmbient(const glm::vec3&) const noexcept;
};

}