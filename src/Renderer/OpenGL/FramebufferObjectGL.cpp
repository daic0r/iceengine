//
//  FramebufferObjectGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 28.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <utility>
#include <iostream>

namespace Ice {

FramebufferObjectGL::FramebufferObjectGL(std::uint16_t w, std::uint16_t h, bool bCreate) noexcept
: m_nWidth{ w }, m_nHeight{ h }
{
	if (bCreate)
		m_nFramebuffer = RenderToolsGL::createFramebuffer();
}

FramebufferObjectGL::~FramebufferObjectGL() {
    if (m_nFramebuffer != 0)
        glCall(glDeleteFramebuffers(1, &m_nFramebuffer));
    if (m_nTexAttachment != 0)
        glCall(glDeleteTextures(1, &m_nTexAttachment));
    if (m_nDepthAttachment != 0)
        glCall(glDeleteRenderbuffers(1, &m_nDepthAttachment));
}

FramebufferObjectGL::FramebufferObjectGL(FramebufferObjectGL&& other) noexcept {
    m_nFramebuffer = std::exchange(other.m_nFramebuffer, 0);
    m_nTexAttachment = std::exchange(other.m_nTexAttachment, 0);
    m_nDepthAttachment = std::exchange(other.m_nDepthAttachment, 0);
    m_nWidth = std::exchange(other.m_nWidth, 0);
    m_nHeight = std::exchange(other.m_nHeight, 0);
}

FramebufferObjectGL& FramebufferObjectGL::operator=(FramebufferObjectGL&& other) noexcept {
    FramebufferObjectGL copy{ std::move(other) };
    copy.swap(*this);
    return *this;
}

bool FramebufferObjectGL::createTextureAttachment() noexcept {
    m_nTexAttachment = RenderToolsGL::createTextureFramebufferAttachment(m_nWidth, m_nHeight);
    return m_nTexAttachment != 0;
}

bool FramebufferObjectGL::createDepthAttachment() noexcept {
    m_nDepthAttachment = RenderToolsGL::createDepthBufferFramebufferAttachment(m_nWidth, m_nHeight);
    return m_nDepthAttachment != 0;
}

bool FramebufferObjectGL::createDepthTextureAttachment() noexcept {
	m_nDepthTextureAttachment = RenderToolsGL::createDepthTextureFramebufferAttachment(m_nWidth, m_nHeight);
	return m_nDepthTextureAttachment != 0;
}

void FramebufferObjectGL::create() noexcept {
	if (m_nFramebuffer == 0)
		m_nFramebuffer = RenderToolsGL::createFramebuffer();
}

void FramebufferObjectGL::bind(GLsizei nOldWidth, GLsizei nOldHeight) noexcept {
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_nLastFBO);
    m_nOldWidth = nOldWidth;
    m_nOldHeight = nOldHeight;
    RenderToolsGL::bindFramebuffer(m_nFramebuffer, m_nWidth, m_nHeight);
}

void FramebufferObjectGL::unbind() noexcept {
    RenderToolsGL::unbindCurrentFramebuffer(m_nLastFBO, m_nOldWidth, m_nOldHeight);
}

void FramebufferObjectGL::swap(FramebufferObjectGL &other) noexcept {
    using std::swap;
    swap(m_nFramebuffer, other.m_nFramebuffer);
    swap(m_nTexAttachment, other.m_nTexAttachment);
    swap(m_nDepthAttachment, other.m_nDepthAttachment);
    swap(m_nWidth, other.m_nWidth);
    swap(m_nHeight, other.m_nHeight);
}

}
