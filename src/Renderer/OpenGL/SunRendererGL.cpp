#include <GL/glad.h>
#include <iostream>
#include <Renderer/OpenGL/SunRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <ShadersGL/SunShaderConfigurator.h>
#include <Renderer/RenderEnvironment.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>
#include <System/Light.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Ice {

const std::vector<glm::vec3> SunRendererGL::PointVertex = { glm::vec3{ 0.0f, 0.0f, 0.0f } };

SunRendererGL::SunRendererGL() {
	m_objPoint = RenderToolsGL::loadVerticesToVAO(SunRendererGL::PointVertex);
	glCall(glEnableVertexAttribArray(0));

	m_pShaderProgram = RenderToolsGL::createShaderProgram("Sun", std::make_unique<SunShaderConfigurator>());
	m_pShaderConfig = dynamic_cast<SunShaderConfigurator*>(m_pShaderProgram->configurator());

	m_pGraphicsSystem = systemServices.getGraphicsSystem();
}

void SunRendererGL::render(const RenderEnvironment& env) const noexcept {
	glCall(glBindVertexArray(m_objPoint.vao()));
	m_pShaderProgram->use();
	m_pShaderConfig->loadUniforms(env);
	m_pShaderConfig->loadPosition(env.pSun->position());
	m_pShaderConfig->loadSunColor(env.pSun->color());
	m_pShaderConfig->loadViewportDimensions(m_pGraphicsSystem->displayWidth(), m_pGraphicsSystem->displayHeight());
	glCall(glEnable(GL_PROGRAM_POINT_SIZE));
	glCall(glEnable(GL_BLEND));
	glCall(glDisable(GL_DEPTH_TEST));
	//glCall(glPointParameterf(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT));
	glCall(glDrawArrays(GL_POINTS, 0, 1));
	glCall(glDisable(GL_PROGRAM_POINT_SIZE));
	//glCall(glEnable(GL_BLEND));
	glCall(glEnable(GL_DEPTH_TEST));
	m_pShaderProgram->unuse();
	glCall(glBindVertexArray(0));
}

}