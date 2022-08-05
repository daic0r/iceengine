//
//  PathSegmentRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathSegmentRendererGL_h
#define PathSegmentRendererGL_h

#include <Entities/Entity.h>
#include <unordered_map>
#include <vector>
#include <Components/PathSegmentComponent.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Interfaces/IPathSegmentRenderer.h>

namespace Ice {

struct RenderEnvironment;
class IShaderProgram;
class Shader3dConfigurator;

class PathSegmentRendererGL : public IPathSegmentRenderer {
  
    std::unordered_map<Entity, std::unique_ptr<RenderObjectGL>> m_mRenderObjects;
    std::unordered_map<Entity, size_t> m_mNumLines;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    Shader3dConfigurator* m_pShaderConfig{ nullptr };
    
public:
    PathSegmentRendererGL();
    void render(const RenderEnvironment&, const std::vector<Entity>&) noexcept override;
};

}

#endif /* PathSegmentRendererGL_h */
