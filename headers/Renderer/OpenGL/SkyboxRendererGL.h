//
//  SkyboxRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SkyboxRendererGL_h
#define SkyboxRendererGL_h

#include <System/Skybox.h>
#include <Interfaces/ISkyboxRenderer.h>
#include <Renderer/CubeTexture.h>
#include <memory>

namespace Ice {

class RenderObjectGL;
class SkyboxShaderConfigurator;
class IShaderProgram;
class ITexture;
class CubeTexture;
struct RenderEnvironment;

class SkyboxRendererGL : public ISkyboxRenderer {
public:
    
    SkyboxRendererGL() noexcept;
    void render(const RenderEnvironment&, float frotation) noexcept override;
    
private:
    
    Skybox m_skybox;
    std::unique_ptr<RenderObjectGL> m_pRenderObject;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    SkyboxShaderConfigurator *m_pShaderConfig{ nullptr };
    std::unique_ptr<CubeTexture> m_pTexture{ nullptr };
    
};

}

#endif /* SkyboxRendererGL_h */
