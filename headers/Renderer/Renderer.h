//
//  Renderer.h
//  CovidSimulator
//
//  Created by Matthias Gruen on 04.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SDLRenderer_h
#define SDLRenderer_h

#include <memory>
#include <unordered_map>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include "../System/Model.h"

class SDL_Window;
class FrustumShaderConfigurator;
class ShaderProgram;
class Scene;
struct Model;
class GLModel;
struct RenderEnvironment;
class IModelRenderer;
class Camera;

class MasterRendererGL {
    MasterRendererGL();
    
public:

    static MasterRendererGL& instance();
    
    bool init();
    bool initGL();
    bool render(float fDeltaTime, Camera* pCamera, const glm::mat4& projectionMatrix, Scene*);
    ~MasterRendererGL();

    SDL_Window* window() const noexcept;
    SDL_GLContext context() const noexcept;
    
    GLuint registerShader(const std::string&, const std::string&, const std::string&);
    void registerModel(Model* pModel);
    ShaderProgram* getShader(GLuint);

private:

    IModelRenderer* m_pModelRenderer{ nullptr };
    std::unordered_map<Model*, std::unique_ptr<GLModel>> m_mModels;
    GLuint m_shaderId{ 0 };
    SDL_Window* m_pWindow{ nullptr };
    SDL_GLContext m_context{ nullptr };
    FrustumShaderConfigurator* m_pFrustumShader{ nullptr };
};

#endif /* SDLRenderer_h */
