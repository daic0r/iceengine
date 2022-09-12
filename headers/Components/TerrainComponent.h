//
//  TerrainComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainComponent_h
#define TerrainComponent_h

#include <IceEngine.h>
#include <glm/vec4.hpp>
#include <string>
#include <vector>
#include <tuple>
#include <Components/ComponentIds.h>
#include <System/PerlinNoise.h>
#include <Entities/Entity.h>
#include <World/LowPolyTerrain.h>
#include <Components/BiomeNodeComponent.h>

namespace Ice {

class ITexture;
class Ray;
class Camera;

struct ICEENGINE_API TerrainComponent {

    static constexpr EntityComponentId id() noexcept { return TERRAIN_COMPONENT; }

    LowPolyTerrain m_terrain;
    std::vector<glm::vec4> m_vOriginalColorBuffer;
    DynamicVertexAttribute* m_pColorAttrib{};
};

}

#endif /* TerrainComponent_h */
