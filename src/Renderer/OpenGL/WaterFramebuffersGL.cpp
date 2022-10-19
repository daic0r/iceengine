//
//  WaterFramebuffersGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//
#ifdef RENDERER_OPEN_GL

#include <glad/glad.h>
#include <Renderer/OpenGL/WaterFramebuffersGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>

namespace Ice {

WaterFramebuffersGL::WaterFramebuffersGL(GLsizei nReflectionWidth, GLsizei nReflectionHeight, GLsizei nRefractionWidth, GLsizei nRefractionHeight)
    : m_reflectionFBO{ nReflectionWidth, nReflectionHeight },
    m_refractionFBO{ nRefractionWidth, nRefractionHeight } 
{
    initReflectionFramebuffer();
    initRefractionFramebuffer();
}

void WaterFramebuffersGL::initReflectionFramebuffer() noexcept {
    m_reflectionFBO.bind();
    m_reflectionFBO.createTextureAttachment();
    m_reflectionFBO.createDepthAttachment();
    m_reflectionFBO.unbind();
}

void WaterFramebuffersGL::resize(GLsizei nWidth, GLsizei nHeight) noexcept {
    m_reflectionFBO.resize(nWidth, nHeight);
    m_refractionFBO.resize(nWidth, nHeight);
}

void WaterFramebuffersGL::initRefractionFramebuffer() noexcept {
    m_refractionFBO.bind();
    m_refractionFBO.createTextureAttachment();
    m_refractionFBO.createDepthTextureAttachment();
    m_refractionFBO.unbind();
}

void WaterFramebuffersGL::bindReflectionFramebuffer() noexcept {
    m_reflectionFBO.bind();
}

void WaterFramebuffersGL::bindRefractionFramebuffer() noexcept {
    m_refractionFBO.bind();
}

}

#endif
