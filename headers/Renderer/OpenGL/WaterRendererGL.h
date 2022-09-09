//
//  WaterRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WaterRendererGL_h
#define WaterRendererGL_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <memory>
#include <vector>
#include "../../Interfaces/IWaterRenderer.h"
#include <Renderer/OpenGL/WaterFramebuffersGL.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <unordered_map>
#include <Components/Systems/TerrainRenderingSystem.h>
#include <Components/Systems/ObjectRenderingSystem.h>


namespace Ice {

class WaterShaderConfigurator;
class RenderObjectGL;
class WaterTile;
struct RenderEnvironment;
class WaterFramebuffersGL;
class TextureGL;
class IPostProcessingEffect;
class IGraphicsSystem;
class ShaderProgramGL;
class OriginalCanvasGL;

class WaterRendererGL : public IWaterRenderer {
    static constexpr float WAVE_SPEED = 0.03;
    
    IGraphicsSystem* m_pGraphicsSystem{};
    std::unique_ptr<ShaderProgramGL> m_pShaderProgram{ nullptr };
    ///////////////////////////////////////////////////////////
    // Uniforms
    GLint m_nPersViewMatrixID{ -1 }, m_nModelMatrixID{ -1 };
    GLint m_nReflectionTextureID{ -1 };
    GLint m_nRefractionTextureID{ -1 };
    GLint m_nRefractionDepthTextureID{ -1 };
    GLint m_nWaterLevelID{ -1 };
    GLint m_nCameraPosID{ -1 };
    GLint m_nDistPlanesID{ -1 };
    GLint m_nGridSizeID{ -1 };
    GLint m_nTimeID{ -1 };
    GLuint m_nCommonMatricesUBOIndex{ 0 };
    ///////////////////////////////////////////////////////////
    float m_fWaveTime{};

    // The actual water tiles
    std::unordered_map<WaterTile*, RenderObjectGL> m_mTileObjects;
    // Reflection and refraction framebuffers
    WaterFramebuffersGL m_fbo;
    // For rendering the parts above the water with the camera below the terrain
    TerrainRenderingSystem *m_pTerrainRenderer{};
    ObjectRenderingSystem* m_pModelRenderer{};
    
    // Original drawing FBO canvas that the complete scene is rendered to
    OriginalCanvasGL* m_pOriginalCanvas{};

    // Water height
    float m_fWaterLevel{};

 #ifdef _DEBUG_WATER
    std::unique_ptr<ShaderProgramGL> m_pPreviewShader{};
    static const std::vector<GLfloat> m_vQuadVertices;
    std::unique_ptr<RenderObjectGL> m_pQuad;
#endif
    
public:
    WaterRendererGL();
    void prepareRendering(const RenderEnvironment&) noexcept override;
    void render(const RenderEnvironment&, const std::vector<WaterTile*>&) noexcept override;
    void finishRendering() noexcept override;
    void setOriginalCanvas(IPostProcessingEffect* pCanvas) noexcept override;
    void setWaterLevel(float f) noexcept override;
    void setGridSize(float f) noexcept override;
    void incWaveTime(float f) noexcept override { m_fWaveTime += f; }
    void resize(int nWidth, int nHeight) noexcept override;
    void renderReflectionTexture(RenderEnvironment env) noexcept;

private:
    RenderObjectGL& registerWaterTile(WaterTile*);
    static const char* getVertexShaderSource() noexcept;
    static const char* getFragmentShaderSource() noexcept;
};

}

#endif

#endif /* WaterRendererGL_h */
