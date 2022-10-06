//
//  SkyboxRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glad.h>
#include <Renderer/OpenGL/SkyboxRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <ShadersGL/SkyboxShaderConfigurator.h>
#include <iostream>
#include <System/Config.h>
#include <Renderer/CubeTexture.h>
#include <System/SystemServices.h>

namespace Ice {

SkyboxRendererGL::SkyboxRendererGL() noexcept {
    m_pRenderObject = std::make_unique<RenderObjectGL>(RenderToolsGL::createVAO());
    
    GLuint nVertexBuffer = RenderToolsGL::loadFloatBuffer(GL_ARRAY_BUFFER, reinterpret_cast<GLfloat*>(&m_skybox.m_mesh.vertices()[0]), m_skybox.m_mesh.vertices().size() * 3, GL_STATIC_DRAW);
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
    glCall(glEnableVertexAttribArray(0));
    
    GLuint nElementBuffer;
    glCall(glGenBuffers(1, &nElementBuffer));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nElementBuffer));
    glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_skybox.m_mesh.indices().size(), &m_skybox.m_mesh.indices()[0], GL_STATIC_DRAW));
    
    m_pRenderObject->addBuffer(nVertexBuffer);
    m_pRenderObject->addBuffer(nElementBuffer);
    
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    glCall(glBindVertexArray(0));
    
    m_pShaderProgram = RenderToolsGL::createShaderProgram(m_skybox.m_mesh.shaderId(), std::make_unique<SkyboxShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<SkyboxShaderConfigurator*>(m_pShaderProgram->configurator());
    
    std::string strPathPrefix = systemServices.getConfig()->prependAssetPath("Skybox_");
//    std::string strPathPrefix{ Config::BasePath };
//    strPathPrefix += "Assets/skybox_";
    m_pTexture = std::make_unique<CubeTexture>();
    m_pTexture->loadFromFile(strPathPrefix);
}

void SkyboxRendererGL::render(const RenderEnvironment& env, float fRotation) noexcept {
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);

    glCall(glDisable(GL_DEPTH_TEST));
    glCall(glDepthFunc(GL_LEQUAL));

    m_pShaderProgram->use();
    m_pShaderConfig->loadUniforms(env);
	m_pShaderConfig->loadRotation(fRotation);
    m_pTexture->bind(m_pShaderConfig->textureUniformId());
    
    glCall(glBindVertexArray(m_pRenderObject->vao()));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pRenderObject->bufferAt(1)));
    
    glCall(glDrawElements(GL_TRIANGLES, m_skybox.m_mesh.indices().size(), GL_UNSIGNED_INT, nullptr));
    
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    glCall(glBindVertexArray(0));
    
    m_pShaderProgram->unuse();
    m_pTexture->unbind();
    
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glDepthFunc(OldDepthFuncMode));
}

}
