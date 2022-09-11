#pragma once

#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Interfaces/ISunRenderer.h>
#include <vector>

namespace Ice {

class SunShaderConfigurator;
class IShaderProgram;
class IGraphicsSystem;

class SunRendererGL : public ISunRenderer {
	static const std::vector<glm::vec3> PointVertex;

	std::unique_ptr<IShaderProgram> m_pShaderProgram;
	SunShaderConfigurator* m_pShaderConfig{ nullptr };
	RenderObjectGL m_objPoint;
	IGraphicsSystem* m_pGraphicsSystem{ nullptr };

public:
	SunRendererGL();
	void render(const RenderEnvironment&) const noexcept override;
};

}