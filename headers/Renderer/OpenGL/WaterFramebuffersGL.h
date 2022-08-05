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
    static constexpr int REFLECTION_WIDTH = 320;
    static constexpr int REFLECTION_HEIGHT = 200;
    
    static constexpr int REFRACTION_WIDTH = 1280;
    static constexpr int REFRACTION_HEIGHT = 720;
    
    GLuint m_nReflectionFramebuffer{ 0 };
    GLuint m_nReflectionTexture{ 0 };
    GLuint m_nReflectionDepthBuffer{ 0 };

    GLuint m_nRefractionFramebuffer{ 0 };
    GLuint m_nRefractionTexture{ 0 };
    GLuint m_nRefractionDepthTexture{ 0 };
    
    void cleanUp() noexcept;
    void initReflectionFramebuffer() noexcept;
    void initRefractionFramebuffer() noexcept;
    
public:
    WaterFramebuffersGL();
    ~WaterFramebuffersGL();
    
    void bindReflectionFramebuffer() const noexcept;
    void bindRefractionFramebuffer() const noexcept;
    
    auto reflectionTexture() const noexcept { return m_nReflectionTexture; }
    auto refractionTexture() const noexcept { return m_nRefractionTexture; }
    auto refractionDepthTexture() const noexcept { return m_nRefractionDepthTexture; }
};

}

#endif


#endif /* WaterFramebuffersGL_h */
