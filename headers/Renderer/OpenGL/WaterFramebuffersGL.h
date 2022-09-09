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

namespace Ice {

class WaterFramebuffersGL {
    static constexpr GLsizei REFLECTION_WIDTH = 320;
    static constexpr GLsizei REFLECTION_HEIGHT = 200;
    
    static constexpr GLsizei REFRACTION_WIDTH = 1280;
    static constexpr GLsizei REFRACTION_HEIGHT = 720;
    
    GLuint m_nReflectionFramebuffer{ 0 };
    GLuint m_nReflectionTexture{ 0 };
    GLuint m_nReflectionDepthBuffer{ 0 };

    GLuint m_nRefractionFramebuffer{ 0 };
    GLuint m_nRefractionTexture{ 0 };
    GLuint m_nRefractionDepthTexture{ 0 };

    GLsizei m_nReflectionWidth{}, m_nReflectionHeight{};
    GLsizei m_nRefractionWidth{}, m_nRefractionHeight{};

    void cleanup() noexcept;
    void initReflectionFramebuffer() noexcept;
    void initRefractionFramebuffer() noexcept;
    
public:
    WaterFramebuffersGL(GLsizei nReflectionWidth = REFLECTION_WIDTH, GLsizei nReflectionHeight = REFLECTION_HEIGHT, GLsizei nRefractionWidth = REFRACTION_WIDTH, GLsizei nRefractionHeight = REFRACTION_HEIGHT);
    ~WaterFramebuffersGL();
    
    void bindReflectionFramebuffer() const noexcept;
    void bindRefractionFramebuffer() const noexcept;
    
    auto reflectionFramebuffer() const noexcept { return m_nReflectionFramebuffer; }
    auto refractionFramebuffer() const noexcept { return m_nRefractionFramebuffer; }
    auto reflectionTexture() const noexcept { return m_nReflectionTexture; }
    auto refractionTexture() const noexcept { return m_nRefractionTexture; }
    auto refractionDepthTexture() const noexcept { return m_nRefractionDepthTexture; }
    auto reflectionWidth() const noexcept { return m_nReflectionWidth; }
    auto reflectionHeight() const noexcept { return m_nReflectionHeight; }
    auto refractionWidth() const noexcept { return m_nRefractionWidth; }
    auto refractionHeight() const noexcept { return m_nRefractionHeight; }
};

}

#endif


#endif /* WaterFramebuffersGL_h */
