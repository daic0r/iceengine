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

FramebufferObjectGL::FramebufferObjectGL(GLsizei w, GLsizei h, bool bCreate) noexcept
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
    if (m_nDepthTextureAttachment != 0)
        glCall(glDeleteTextures(1, &m_nDepthTextureAttachment));
    m_nFramebuffer = m_nTexAttachment = m_nDepthAttachment = m_nDepthTextureAttachment = 0;
}

FramebufferObjectGL::FramebufferObjectGL(FramebufferObjectGL&& other) noexcept {
    m_nFramebuffer = std::exchange(other.m_nFramebuffer, 0);
    m_nTexAttachment = std::exchange(other.m_nTexAttachment, 0);
    m_nDepthAttachment = std::exchange(other.m_nDepthAttachment, 0);
    m_nDepthTextureAttachment = std::exchange(other.m_nDepthTextureAttachment, 0);
    m_nWidth = std::exchange(other.m_nWidth, 0);
    m_nHeight = std::exchange(other.m_nHeight, 0);
    m_nOldWidth = std::exchange(other.m_nOldWidth, 0);
    m_nOldHeight = std::exchange(other.m_nOldHeight, 0);
    m_nLastFBO = std::exchange(other.m_nLastFBO, 0);
}

FramebufferObjectGL& FramebufferObjectGL::operator=(FramebufferObjectGL&& other) noexcept {
    FramebufferObjectGL copy{ std::move(other) };
    copy.swap(*this);
    return *this;
}

bool FramebufferObjectGL::createTextureAttachment() noexcept {
    if (m_nTexAttachment == 0)
        m_nTexAttachment = RenderToolsGL::createTextureFramebufferAttachment(m_nWidth, m_nHeight);
    return m_nTexAttachment != 0;
}

bool FramebufferObjectGL::createDepthAttachment() noexcept {
    if (m_nDepthAttachment == 0)
        m_nDepthAttachment = RenderToolsGL::createDepthBufferFramebufferAttachment(m_nWidth, m_nHeight);
    return m_nDepthAttachment != 0;
}

bool FramebufferObjectGL::createDepthTextureAttachment() noexcept {
    if (m_nDepthTextureAttachment == 0)
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

void FramebufferObjectGL::resize(GLsizei nWidth, GLsizei nHeight) noexcept {
    const auto bTexture = m_nTexAttachment != 0;
    const auto bDepth = m_nDepthAttachment != 0;
    const auto bDepthTexture = m_nDepthTextureAttachment != 0;
    this->~FramebufferObjectGL();
    setWidth(nWidth);
    setHeight(nHeight);
    create();
    if (bTexture)
        createTextureAttachment();
    if (bDepth)
        createDepthAttachment();
    if (bDepthTexture)
        createDepthTextureAttachment();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FramebufferObjectGL::swap(FramebufferObjectGL &other) noexcept {
    using std::swap;
    swap(m_nFramebuffer, other.m_nFramebuffer);
    swap(m_nTexAttachment, other.m_nTexAttachment);
    swap(m_nDepthAttachment, other.m_nDepthAttachment);
    swap(m_nDepthTextureAttachment, other.m_nDepthTextureAttachment);
    swap(m_nWidth, other.m_nWidth);
    swap(m_nHeight, other.m_nHeight);
    swap(m_nOldWidth, other.m_nOldWidth);
    swap(m_nOldHeight, other.m_nOldHeight);
    swap(m_nLastFBO, other.m_nLastFBO);
}

}
