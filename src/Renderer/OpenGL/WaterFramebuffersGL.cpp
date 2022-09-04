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
    cleanup();
}

void WaterFramebuffersGL::cleanup() noexcept {
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
    m_nReflectionFramebuffer = m_nReflectionTexture = m_nReflectionDepthBuffer = m_nRefractionFramebuffer = m_nRefractionTexture = m_nRefractionDepthTexture = 0;
}

WaterFramebuffersGL::WaterFramebuffersGL(GLsizei nReflectionWidth, GLsizei nReflectionHeight, GLsizei nRefractionWidth, GLsizei nRefractionHeight)
    : m_nReflectionWidth{ nReflectionWidth },
    m_nReflectionHeight{ nReflectionHeight },
    m_nRefractionWidth{ nRefractionWidth },
    m_nRefractionHeight{ nRefractionHeight }
{
    
    initReflectionFramebuffer();
    initRefractionFramebuffer();
}

void WaterFramebuffersGL::initReflectionFramebuffer() noexcept {
    m_nReflectionFramebuffer = RenderToolsGL::createFramebuffer();
    m_nReflectionTexture = RenderToolsGL::createTextureFramebufferAttachment(m_nReflectionWidth, m_nReflectionHeight);
    m_nReflectionDepthBuffer = RenderToolsGL::createDepthBufferFramebufferAttachment(m_nReflectionWidth, m_nReflectionHeight);
    RenderToolsGL::unbindCurrentFramebuffer();
}

void WaterFramebuffersGL::initRefractionFramebuffer() noexcept {
    m_nRefractionFramebuffer = RenderToolsGL::createFramebuffer();
    m_nRefractionTexture = RenderToolsGL::createTextureFramebufferAttachment(m_nRefractionWidth, m_nRefractionHeight);
    m_nRefractionDepthTexture = RenderToolsGL::createDepthTextureFramebufferAttachment(m_nRefractionWidth, m_nRefractionHeight);
    RenderToolsGL::unbindCurrentFramebuffer();
}

void WaterFramebuffersGL::bindReflectionFramebuffer() const noexcept {
    RenderToolsGL::bindFramebuffer(m_nReflectionFramebuffer, m_nReflectionWidth, m_nReflectionHeight);
}

void WaterFramebuffersGL::bindRefractionFramebuffer() const noexcept {
    RenderToolsGL::bindFramebuffer(m_nRefractionFramebuffer, m_nRefractionWidth, m_nRefractionHeight);
}

}

#endif
