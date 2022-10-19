//
//  RenderObjectGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifdef RENDERER_OPEN_GL

#include <glad/glad.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <utility>
#include <iostream>

namespace Ice {

RenderObjectGL::~RenderObjectGL() {
    for(auto nBuf : m_vIndexBuffers) {
        if (nBuf != 0)
            glCall(glDeleteBuffers(1, &nBuf));
    }
    for(auto nBuf : m_vBuffers) {
        if (nBuf != 0)
            glCall(glDeleteBuffers(1, &nBuf));
    }
    if (m_VAO != 0) {
        glCall(glDeleteVertexArrays(1, &m_VAO));
    }
}

void RenderObjectGL::swap(RenderObjectGL& b) noexcept {
    using std::swap;
    swap(m_VAO, b.m_VAO);
    swap(m_vBuffers, b.m_vBuffers);
    swap(m_vIndexBuffers, b.m_vIndexBuffers);
    swap(m_vTextures, b.m_vTextures);
    swap(m_pShader, b.m_pShader);
    swap(m_pShaderConfig, b.m_pShaderConfig);
}

RenderObjectGL::RenderObjectGL(RenderObjectGL&& other) noexcept {
    m_VAO = std::exchange(other.m_VAO, 0);
    m_vBuffers = std::move(other.m_vBuffers);
    m_vIndexBuffers = std::move(other.m_vIndexBuffers);
    m_vTextures = std::move(other.m_vTextures);
    m_pShader = std::exchange(other.m_pShader, nullptr);
    m_pShaderConfig = std::exchange(other.m_pShaderConfig, nullptr);
}

RenderObjectGL& RenderObjectGL::operator=(RenderObjectGL &&other) noexcept {
    RenderObjectGL copy{ std::move(other) };
    copy.swap(*this);
    return *this;
}

GLuint RenderObjectGL::createEmptyVBO(int nNumFloats) noexcept {
    GLuint nRet = RenderToolsGL::createEmptyVBO(m_VAO, nNumFloats);
    m_vBuffers.emplace_back(nRet);
    return nRet;
}

void RenderObjectGL::addInstanceVertexAttribute(GLuint nVbo, int nAttribIndex, int nNumComponents, int nInstanceDataChunkLength, int nNumFloatsOffset) noexcept {
    RenderToolsGL::addInstanceVertexAttribute(nVbo, nAttribIndex, nNumComponents, nInstanceDataChunkLength, nNumFloatsOffset);
}

void RenderObjectGL::addBuffer(GLuint nVbo) noexcept{
    m_vBuffers.emplace_back(nVbo);
}

void RenderObjectGL::addIndexBuffer(GLuint nIbo, std::size_t nNumIndices) noexcept {
    m_vIndexBuffers.emplace_back(nIbo);
    m_nNumIndices = nNumIndices;
}

GLuint RenderObjectGL::bufferAt(int nIndex) {
    if (nIndex > m_vBuffers.size() - 1)
        throw std::runtime_error("Index out of bounds");
    
    return m_vBuffers.at(nIndex);
}

GLuint RenderObjectGL::indexBufferAt(int nIndex) {
    if (nIndex > m_vIndexBuffers.size() - 1)
        throw std::runtime_error("Index out of bounds");
    
    return m_vIndexBuffers.at(nIndex);
}

}

#endif
