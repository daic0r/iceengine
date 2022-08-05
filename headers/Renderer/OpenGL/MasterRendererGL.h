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
#include "../../System/Model.h"
#include <Interfaces/IMasterRenderer.h>
#include <Interfaces/IGraphicsSystem.h>

class SDL_Window;

namespace Ice {

class FrustumShaderConfigurator;
class ShaderProgramGL;
class Scene;
struct Model;
class GLModel;
struct RenderEnvironment;
class IModelRenderer;
class Camera;
class IWaterRenderer;
class WaterFramebuffersGL;
class SkyboxGLModel;
class ITerrainRenderer;
class TerrainRenderingSystem;
class ObjectRenderingSystem;

class GraphicsSystemGL : public IGraphicsSystem {
private:
    static bool m_bInstanced;
    
public:
    GraphicsSystemGL();
   
    bool init() override;
    bool render(float fDeltaTime, Camera* pCamera) override;
    void registerModel(Model* pModel) override;
    void resizeWindow(int width, int height) noexcept override;
    inline const glm::mat4& projectionMatrix() const noexcept override { return m_projectionMatrix; }
    inline int displayWidth() const noexcept override { return m_nWidth; }
    inline int displayHeight() const noexcept override { return m_nHeight; }
    inline float fov() const noexcept override { return m_fFoV; }
    ~GraphicsSystemGL();

    SDL_Window* window() const noexcept;
    SDL_GLContext context() const noexcept;

private:
    bool initGL();

    void renderSkybox(const RenderEnvironment&);
    void renderScene(const RenderEnvironment&);
    
    void initSkybox();
    
    void updateProjectionMatrix() noexcept;
    
    IModelRenderer* m_pModelRenderer{ nullptr };
    IWaterRenderer* m_pWaterRenderer{ nullptr };
    ITerrainRenderer* m_pTerrainRenderer{ nullptr };
    std::unique_ptr<WaterFramebuffersGL> m_pWaterFramebuffers;
    std::unique_ptr<TerrainRenderingSystem> m_pTerrainRenderingSystem;
    std::unique_ptr<ObjectRenderingSystem> m_pObjectRenderingSystem;
    
    glm::mat4 m_projectionMatrix;
    float m_fFoV{ 45.0f };
    int m_nWidth, m_nHeight;
    
    GLuint m_crossHairVAO, m_crossHairVBO, m_crossHair_IBO;
    std::unique_ptr<SkyboxGLModel> m_pSkybox;
    std::unordered_map<Model*, std::unique_ptr<GLModel>> m_mModels;
    GLuint m_shaderId{ 0 };
    SDL_Window* m_pWindow{ nullptr };
    SDL_GLContext m_context{ nullptr };
    //FrustumShaderConfigurator* m_pFrustumShader{ nullptr };
};

}

#endif /* SDLRenderer_h */
