//
//  TerrainRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainRendererGL_h
#define TerrainRendererGL_h

#include <map>
#include "RenderObjectGL.h"
#include <Interfaces/ITerrainRenderer.h>
#include <array>
#include <memory>
#include <Renderer/OpenGL/VertexAttributeGL.h>
#include <Renderer/OpenGL/VAO.h>

#ifdef _DEBUG_OCTREE
#include <Renderer/OpenGL/OctreeDebuggerGL.h>
#endif

namespace Ice {

struct RenderEnvironment;
struct Terrain;
class IShaderProgram;
class TerrainShaderConfigurator;
class TerrainHighlightShaderConfigurator;
class RenderObjectGL;
class ModelRendererGL;
class GraphicsSystemGL;
class IOctreeTraversal;

class TerrainRendererGL : public ITerrainRenderer {
    static constexpr std::array<GLfloat, 2*4> HighlightQuad {
        0.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    
    GLuint m_quadVAO;
    GLuint m_quadVBO;
        
    //std::map<int, std::unique_ptr<RenderObjectGL>> m_mTerrains;
    std::map<int, VAO> m_mTerrains;
    
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    TerrainShaderConfigurator *m_pShaderConfig{ nullptr };

    std::unique_ptr<IShaderProgram> m_pTerrainHighlightShaderProgram{ nullptr };
    TerrainHighlightShaderConfigurator *m_pTerrainHighlightShaderConfig{ nullptr };

	ModelRendererGL* m_pModelRenderer{ nullptr };
    GraphicsSystemGL* m_pGraphicsSystem{};

    std::unique_ptr<DynamicVertexAttributeGL<glm::vec4>> m_pVertexColors;

    // Debug
#ifdef _DEBUG_OCTREE
    IOctreeTraversal *m_pOctree{};
    std::unique_ptr<OctreeDebuggerGL> m_pOctreeDebugger;
#endif


public:
    TerrainRendererGL();
    ~TerrainRendererGL();
    
    void prepareRendering(const RenderEnvironment&, const std::vector<Terrain>&) noexcept override;
    void render(const RenderEnvironment&, const std::vector<Terrain>&) noexcept override;
    void finishRendering(const RenderEnvironment&) noexcept override;
#ifdef _DEBUG_OCTREE
    void setOctree(IOctreeTraversal* pOc) noexcept override { m_pOctree = pOc; }
#endif
};

}

#endif /* TerrainRendererGL_h */
