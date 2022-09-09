//
//  WaterFramebuffersGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WaterFramebuffersGL_h
#define WaterFramebuffersGL_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <Renderer/OpenGL/FramebufferObjectGL.h>

namespace Ice {

class WaterFramebuffersGL {
    static constexpr GLsizei REFLECTION_WIDTH = 320;
    static constexpr GLsizei REFLECTION_HEIGHT = 200;
    
    static constexpr GLsizei REFRACTION_WIDTH = 1280;
    static constexpr GLsizei REFRACTION_HEIGHT = 720;
    
    FramebufferObjectGL m_reflectionFBO, m_refractionFBO;

    void cleanup() noexcept;
    void initReflectionFramebuffer() noexcept;
    void initRefractionFramebuffer() noexcept;
    
public:
    WaterFramebuffersGL(GLsizei nReflectionWidth = REFLECTION_WIDTH, GLsizei nReflectionHeight = REFLECTION_HEIGHT, GLsizei nRefractionWidth = REFRACTION_WIDTH, GLsizei nRefractionHeight = REFRACTION_HEIGHT);
    
    void bindReflectionFramebuffer() noexcept;
    void bindRefractionFramebuffer() noexcept;

    void resize(GLsizei nWidth, GLsizei nHeight) noexcept;
    
    auto reflectionFramebuffer() const noexcept { return m_reflectionFBO.fboId(); }
    auto refractionFramebuffer() const noexcept { return m_refractionFBO.fboId(); }
    auto reflectionTexture() const noexcept { return m_reflectionFBO.textureAttachmentId(); }
    auto refractionTexture() const noexcept { return m_refractionFBO.textureAttachmentId(); }
    auto refractionDepthTexture() const noexcept { return m_refractionFBO.depthTextureAttachmentId(); }
    auto reflectionWidth() const noexcept { return m_reflectionFBO.width(); }
    auto reflectionHeight() const noexcept { return m_reflectionFBO.height(); }
    auto refractionWidth() const noexcept { return m_refractionFBO.width(); }
    auto refractionHeight() const noexcept { return m_refractionFBO.height(); }
};

}

#endif


#endif /* WaterFramebuffersGL_h */
