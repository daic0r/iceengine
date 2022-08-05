//
//  RenderObjectGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef RenderObjectGL_h
#define RenderObjectGL_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <vector>
#include <map>
#include <Interfaces/IShaderProgram.h>

namespace Ice {

class IShaderConfigurator;
class TextureGL;

class RenderObjectGL {
    GLuint m_VAO{ 0 };
    std::vector<GLuint> m_vBuffers;
    std::vector<GLuint> m_vIndexBuffers;
    std::vector<TextureGL*> m_vTextures;
    IShaderProgram *m_pShader{nullptr};
    IShaderConfigurator *m_pShaderConfig{nullptr};
    
public:
	RenderObjectGL() = default;
    RenderObjectGL(GLuint vao) : m_VAO{ vao } {}
    RenderObjectGL(const RenderObjectGL&) = delete;
    RenderObjectGL& operator=(const RenderObjectGL&) = delete;
    RenderObjectGL(RenderObjectGL&&) noexcept;
    RenderObjectGL& operator=(RenderObjectGL&&) noexcept;
    
    virtual ~RenderObjectGL();
    
    GLuint vao() const noexcept { return m_VAO; }
    
    GLuint createEmptyVBO(int nNumFloats) noexcept;
    void addInstanceVertexAttribute(GLuint nVbo, int nAttribIndex, int nNumComponents, int nInstanceDataChunkLength, int nNumFloatsOffset) noexcept;
    void addBuffer(GLuint nVbo) noexcept;
    void addIndexBuffer(GLuint nIbo) noexcept;
    GLuint bufferAt(int);
    GLuint indexBufferAt(int);
    
    inline void setShaderProgram(IShaderProgram* pProgram) noexcept { m_pShader = pProgram; }
    inline void setShaderConfigurator(IShaderConfigurator* pShaderConfig) noexcept { m_pShaderConfig = pShaderConfig; }
    
    inline auto shaderProgram() const noexcept { return m_pShader; }
    inline auto shaderConfigurator() const noexcept { return m_pShaderConfig; }
    
    inline auto textureAt(int n) const { return m_vTextures.at(n); }
    inline void addTexture(TextureGL* pTex) noexcept { m_vTextures.emplace_back(pTex); }
    
    void swap(RenderObjectGL& b) noexcept;
    friend void swap(RenderObjectGL& a, RenderObjectGL& b) noexcept {
        a.swap(b);
    }
    
};

}

#endif

#endif /* RenderObjectGL_h */
