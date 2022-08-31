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

namespace Ice {

class WaterShaderConfigurator;
class IShaderProgram;
class RenderObjectGL;
class WaterTile;
struct RenderEnvironment;
class WaterFramebuffersGL;
class TextureGL;

class WaterRendererGL : public IWaterRenderer {
    static constexpr float WAVE_SPEED = 0.03;
    
    static const std::vector<GLfloat> m_vQuadVertices;
    std::unique_ptr<RenderObjectGL> m_pQuad;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    /*WaterShaderConfigurator *m_pShaderConfig{ nullptr };
    TextureGL *m_pDuDvTexture{ nullptr }, *m_pNormalTexture{ nullptr };
    float m_fMoveFactor{ 0.0f };
    std::unique_ptr<WaterFramebuffersGL> m_pFramebuffers;
    */
    GLint m_nPersViewMatrixID{ -1 }, m_nModelMatrixID{ -1 };
    std::unordered_map<WaterTile*, RenderObjectGL> m_mTileObjects;
    
public:
    WaterRendererGL();
    void prepareRendering(const RenderEnvironment&) noexcept override;
    void render(const RenderEnvironment&, const std::vector<WaterTile*>&) noexcept override;
    void finishRendering() noexcept override;

private:
    RenderObjectGL& registerWaterTile(WaterTile*);
    static const char* getVertexShaderSource() noexcept;
    static const char* getFragmentShaderSource() noexcept;
};

}

#endif

#endif /* WaterRendererGL_h */
