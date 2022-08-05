//
//  Renderer.h
//  CovidSimulator
//
//  Created by Matthias Gruen on 04.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SDLRenderer_h
#define SDLRenderer_h

#include "../Interfaces/IRenderer.h"
#include <memory>

class SDL_Renderer;
class SDL_Window;
class Shader;

class Renderer {
    std::unique_ptr<Shader> m_shaderProgram;

    Renderer();
    
public:

    static Renderer& instance();
    
    bool init();
    bool render();
    ~Renderer();
    
private:
    bool initGL();
    
    SDL_Window* window() const noexcept;
    SDL_Renderer* renderer() const noexcept;
    
    SDL_Window* m_pWindow{ nullptr };
    SDL_Renderer* m_pRenderer{ nullptr };
};

#endif /* SDLRenderer_h */
