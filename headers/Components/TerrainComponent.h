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

namespace Ice {

class ITexture;
class Ray;
class Camera;

struct ICEENGINE_API TerrainComponent {

    static constexpr EntityComponentId id() noexcept { return TERRAIN_COMPONENT; }
    LowPolyTerrain m_terrain;
    std::vector<glm::vec4> m_vOriginalColorBuffer;
/*
public:
    
    static constexpr float SIZE = 256.0f;
    static constexpr int NUM_PARTS_PER_DIMENSION = 256;
    
    TerrainComponent(int nGridX, int nGridZ, float fSize = 256.0f, int nNumPartsPerDimension = 256) noexcept;
    
    inline auto terrainId() const noexcept { return m_nId; }
    bool getHeight(float x, float z, float& fOutHeight) const noexcept;
    std::pair<int, int> getSelectedTile(const Camera& cam, float fHeightOffset, int nSize) const noexcept;

    inline float worldX() const noexcept { return m_fWorldX; }
    inline float worldZ() const noexcept { return m_fWorldZ; }
    inline float tileSize() const noexcept { return m_fTileSize; }
    float size() const noexcept { return m_fSize; }
    int numTiles() const noexcept { return m_nNumPartsPerDimension; }
    bool getCenterCoordsForTile(int x, int z, float& outX, float& outZ) const noexcept;
    inline std::vector<std::vector<float>>& heights() noexcept { return m_vHeights; }
    
    static constexpr EntityComponentId id() noexcept { return TERRAIN_COMPONENT; }
    
private:
    static int nCurrentId;
    int m_nId{ -1 };
    
    float m_fWorldX{ -1 }, m_fWorldZ{ -1 };
    float m_fTileSize{ 0.0f };
    std::vector<std::vector<float>> m_vHeights;
    
    float m_fSize{ 256.0f };
    int m_nNumPartsPerDimension{ 256 };
    
    static std::tuple<float, float, float> getBarycentricCoords(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v) noexcept;
    std::pair<int, int> getTerrainTileFromRay(const Ray&, float fHeightOffset = 0.0f, float fRayLength = 300.0f, int nRecurseLevel = 0) const noexcept;
*/
};

}

#endif /* TerrainComponent_h */
