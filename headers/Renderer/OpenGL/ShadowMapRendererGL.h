#pragma once

#include <Renderer/OpenGL/ModelRendererGL.h>
#include <Renderer/OpenGL/CommonMatrixUBO_GL.h>
#include <Interfaces/IShadowMapRenderer.h>

namespace Ice {

class ShadowMapRendererGL : public ModelRendererGL, public IShadowMapRenderer {

	std::unique_ptr<IShaderProgram> m_pShadowProgram;
	ShadowShaderConfigurator* m_pShadowShaderConfig{ nullptr };
	FramebufferObjectGL m_shadowFbo;
	glm::mat4 m_shadowProjectionViewMatrix;

	// For rendering the shadow map for debugging purposes
	std::unique_ptr<IShaderProgram> m_pShadowTexDisplayerProgram{ nullptr };
	IShaderConfigurator* m_pShadowTexDisplayerConfig{ nullptr };
	RenderObjectGL m_shadowTexObj;

public:
	static constexpr float shadowMapDimension() noexcept { return 4096.0f; }

	ShadowMapRendererGL();
	void renderShadowDepthTexture() const noexcept override;
	GLuint getShadowDepthTextureId() const noexcept;
	const glm::mat4& shadowProjectionViewMatrix() const noexcept { return m_shadowProjectionViewMatrix; }
	void clear() noexcept;

private:
	void prepareRendering(const RenderEnvironment&) noexcept override;
    void finishRendering() noexcept override;
	void prepareShader(RenderObjectGL* pModel, const RenderEnvironment& env) noexcept override;
	void unbindShader(RenderObjectGL* pModel) noexcept override;
	void setRenderMaterial(const RenderMaterial&, ModelShaderConfigurator* pConfig) noexcept override;
};

}