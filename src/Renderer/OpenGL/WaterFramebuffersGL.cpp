//
//  WaterFramebuffersGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//
#ifdef RENDERER_OPEN_GL

#include <GL/glew.h>
#include <Renderer/OpenGL/WaterFramebuffersGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>

namespace Ice {

WaterFramebuffersGL::~WaterFramebuffersGL() {
    cleanUp();
}

void WaterFramebuffersGL::cleanUp() noexcept {
    if (m_nReflectionFramebuffer != 0)
        glCall(glDeleteFramebuffers(1, &m_nReflectionFramebuffer));
    if (m_nReflectionTexture != 0)
        glCall(glDeleteTextures(1, &m_nReflectionTexture));
    if (m_nReflectionDepthBuffer != 0)
        glCall(glDeleteRenderbuffers(1, &m_nReflectionDepthBuffer));
    if (m_nRefractionFramebuffer != 0)
        glCall(glDeleteFramebuffers(1, &m_nRefractionFramebuffer));
    if (m_nRefractionTexture != 0)
        glCall(glDeleteTextures(1, &m_nRefractionTexture));
    if (m_nRefractionDepthTexture != 0)
        glCall(glDeleteTextures(1, &m_nRefractionDepthTexture));
}

WaterFramebuffersGL::WaterFramebuffersGL() {
    initReflectionFramebuffer();
    initRefractionFramebuffer();
}

void WaterFramebuffersGL::initReflectionFramebuffer() noexcept {
    m_nReflectionFramebuffer = RenderToolsGL::createFramebuffer();
    m_nReflectionTexture = RenderToolsGL::createTextureFramebufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    m_nReflectionDepthBuffer = RenderToolsGL::createDepthBufferFramebufferAttachment(REFLECTION_WIDTH, REFLECTION_HEIGHT);
    RenderToolsGL::unbindCurrentFramebuffer();
}

void WaterFramebuffersGL::initRefractionFramebuffer() noexcept {
    m_nRefractionFramebuffer = RenderToolsGL::createFramebuffer();
    m_nRefractionTexture = RenderToolsGL::createTextureFramebufferAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    m_nRefractionDepthTexture = RenderToolsGL::createDepthTextureFramebufferAttachment(REFRACTION_WIDTH, REFRACTION_HEIGHT);
    RenderToolsGL::unbindCurrentFramebuffer();
}

void WaterFramebuffersGL::bindReflectionFramebuffer() const noexcept {
    RenderToolsGL::bindFramebuffer(m_nReflectionFramebuffer, REFLECTION_WIDTH, REFLECTION_HEIGHT);
}

void WaterFramebuffersGL::bindRefractionFramebuffer() const noexcept {
    RenderToolsGL::bindFramebuffer(m_nRefractionFramebuffer, REFRACTION_WIDTH, REFRACTION_HEIGHT);
}

}

#endif
