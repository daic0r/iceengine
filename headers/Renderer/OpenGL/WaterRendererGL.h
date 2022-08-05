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

namespace Ice {

class WaterShaderConfigurator;
class IShaderProgram;
class RenderObjectGL;
class WaterTile;
struct RenderEnvironment;
class WaterFrambebuffersGL;
class TextureGL;

class WaterRendererGL : public IWaterRenderer {
    static constexpr float WAVE_SPEED = 0.03;
    
    static const std::vector<GLfloat> m_vQuadVertices;
    std::unique_ptr<RenderObjectGL> m_pQuad;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    WaterShaderConfigurator *m_pShaderConfig{ nullptr };
    TextureGL *m_pDuDvTexture{ nullptr }, *m_pNormalTexture{ nullptr };
    float m_fMoveFactor{ 0.0f };
    
public:
    WaterRendererGL();
    void prepareRendering(const RenderEnvironment&) noexcept override;
    void render(const RenderEnvironment&, const std::vector<WaterTile*>&) noexcept override;
    void finishRendering() noexcept override;
};

}

#endif

#endif /* WaterRendererGL_h */
