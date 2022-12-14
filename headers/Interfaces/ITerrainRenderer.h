//
//  ITerrainRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ITerrainRenderer_h
#define ITerrainRenderer_h

#include <vector>
#include <optional>

namespace Ice {

struct Terrain;
struct RenderEnvironment;
#ifdef _DEBUG_OCTREE
class IOctreeTraversal;
#endif

class ITerrainRenderer {
public:
    virtual void prepareRendering(const RenderEnvironment&, const std::vector<Terrain>&) noexcept = 0;
    virtual void render(const RenderEnvironment&, const std::vector<Terrain>&) noexcept = 0;
    virtual void finishRendering(const RenderEnvironment&) noexcept = 0;
#ifdef _DEBUG_OCTREE
    virtual void setOctree(IOctreeTraversal*) noexcept = 0;
#endif
    virtual ~ITerrainRenderer() = default;
};

}

#endif /* ITerrainRenderer_h */
