#include <GL/glew.h>
#include <Renderer/OpenGL/ShadowMapRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <Renderer/Frustum.h>
#include <Renderer/RenderEnvironment.h>
#include <System/Light.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ShadersGL/ShadowShaderConfigurator.h>
#include <Interfaces/IGraphicsSystem.h>
#include <System/SystemServices.h>
#include <iostream>
#include <ShadersGL/TexDisplayShaderConfigurator.h>
#include <Renderer/OpenGL/GraphicsSystemGL.h>

namespace Ice {

ShadowMapRendererGL::ShadowMapRendererGL() {
	m_shadowFbo.setWidth(static_cast<int>(shadowMapDimension()));
	m_shadowFbo.setHeight(static_cast<int>(shadowMapDimension()));
	m_shadowFbo.create();
	m_shadowFbo.createDepthTextureAttachment();
	glCall(glDrawBuffer(GL_NONE));
	glCall(glReadBuffer(GL_NONE));
	m_shadowFbo.unbind();
	
	std::vector<glm::vec2> arTexVerts = { 
		glm::vec2{ 0.0f, 0.0f },
		glm::vec2{ 0.0f, 1.0f },
		glm::vec2{ 1.0f, 0.0f },
		glm::vec2{ 1.0f, 1.0f }
	};
	m_shadowTexObj = RenderToolsGL::loadVerticesToVAO(arTexVerts);
	glCall(glEnableVertexAttribArray(0));
	glCall(glBindVertexArray(0));

	m_pShadowProgram = RenderToolsGL::createShaderProgram("Shadow", std::make_unique<ShadowShaderConfigurator>());
	m_pShadowShaderConfig = dynamic_cast<ShadowShaderConfigurator*>(m_pShadowProgram->configurator());
	m_pShadowTexDisplayerProgram = RenderToolsGL::createShaderProgram("TexDisplay", std::make_unique<TexDisplayShaderConfigurator>());
	m_pShadowTexDisplayerConfig = m_pShadowTexDisplayerProgram->configurator();
	m_pGraphicsSystem = dynamic_cast<GraphicsSystemGL*>(systemServices.getGraphicsSystem());

	m_pGraphicsSystem->commonUBO().bind();
	// If a fragment is farther away from the camera than this, shadows won't be
	// taken into account anymore
	m_pGraphicsSystem->commonUBO().loadShadowDistance(m_pGraphicsSystem->distFarPlane() / 2.0f);
	m_pGraphicsSystem->commonUBO().loadShadowMargin(10.0f);
	m_pGraphicsSystem->commonUBO().loadShadowTexelSize(1.0f / shadowMapDimension());
	m_pGraphicsSystem->commonUBO().unbind();
}

void ShadowMapRendererGL::prepareRendering(const RenderEnvironment&) noexcept {
	m_shadowFbo.bind();
	glCall(glEnable(GL_DEPTH_TEST));
	//glCall(glClear(GL_DEPTH_BUFFER_BIT));
}

void ShadowMapRendererGL::finishRendering(const RenderEnvironment&) noexcept {
	m_shadowFbo.unbind();
	glCall(glDisable(GL_DEPTH_TEST));
}

void ShadowMapRendererGL::prepareShader(RenderObjectGL* pModel, const RenderEnvironment& env) noexcept {
	if (env.pSun == nullptr)
		return;
	m_pShadowProgram->use();
	// Created shortened frustum around which we'll fit an orthographic view frustum ("shadow box")
	Frustum f{ *env.pCamera, m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distFarPlane() / 2.0f, glm::radians(m_pGraphicsSystem->fov()), m_pGraphicsSystem->aspectRatio() };
	const auto lightViewMatrix = env.pSun->getDirectionalLightViewMatrix(f);
	const auto projMatrix = f.getShadowProjection(lightViewMatrix, 10.0f);

	m_shadowProjectionViewMatrix = projMatrix * lightViewMatrix;
	//m_pShadowShaderConfig->loadProjectionViewMatrix(m_shadowProjectionViewMatrix);
	m_pGraphicsSystem->commonUBO().bind();
	m_pGraphicsSystem->commonUBO().loadShadowProjViewMatrix(m_shadowProjectionViewMatrix);
	m_pGraphicsSystem->commonUBO().unbind();
}

void ShadowMapRendererGL::unbindShader(RenderObjectGL* pModel, const RenderEnvironment&) noexcept {
	glCall(m_pShadowProgram->unuse());
}

void ShadowMapRendererGL::setRenderMaterial(const RenderMaterial& mat, ModelShaderConfigurator* pConfig) noexcept {
}

GLuint ShadowMapRendererGL::getShadowDepthTextureId() const noexcept {
	return m_shadowFbo.depthTextureAttachmentId();
}

void ShadowMapRendererGL::clear() noexcept {
	m_shadowFbo.bind();
	glCall(glClear(GL_DEPTH_BUFFER_BIT));
	m_shadowFbo.unbind();
}

// Debug method to display the shadow map
void ShadowMapRendererGL::renderShadowDepthTexture() const noexcept {
	glCall(glBindVertexArray(m_shadowTexObj.vao()));
	m_pShadowTexDisplayerProgram->use();
	glCall(glActiveTexture(GL_TEXTURE0));
	glCall(glBindTexture(GL_TEXTURE_2D, m_shadowFbo.depthTextureAttachmentId()));
	glCall(glDisable(GL_DEPTH_TEST));
	glCall(glDisable(GL_BLEND));
	glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
	glCall(glEnable(GL_DEPTH_TEST));
	glCall(glEnable(GL_BLEND));
	glCall(glBindTexture(GL_TEXTURE_2D, 0));
	m_pShadowTexDisplayerProgram->unuse();
	glCall(glBindVertexArray(0));
}

}