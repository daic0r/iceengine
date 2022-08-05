//
//  Terrain.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Terrain_h
#define Terrain_h

#include <Components/TerrainComponent.h>
#include <Components/TextureComponent.h>
#include <Components/MeshComponent.h>

namespace Ice {

struct Terrain {
    TerrainComponent *pTerrain{ nullptr };
    TextureComponent *pTexture{ nullptr };
    MeshComponent *pMesh{ nullptr };
};

}

/*
namespace Deprecated {

#include <string>
#include <vector>
#include <array>
#include <System/PerlinNoise.h>
#include <glm/vec2.hpp>
#include <GUI/MousePicker.h>

class ITexture;
class Camera;
class Ray;

class Terrain {
    friend class TerrainRendererGL;
    friend class Controller;
    
    static int nCurrentId;
    
    int m_nId{ -1 };
    ITexture* m_pTexture{ nullptr };
    std::string m_strTextureName;
    float m_fWorldX{ -1 }, m_fWorldZ{ -1 };
    std::vector<float> m_vVertices;
    std::vector<float> m_vTexCoords;
    std::vector<float> m_vNormals;
    std::vector<unsigned int> m_vIndices;
    PerlinNoise m_noise{ 1337 };
    float m_fTileSize{ 0.0f };
    std::vector<std::vector<float>> m_vHeights;

public:
    static constexpr float SIZE = 2048.0f;
    static constexpr int NUM_PARTS_PER_DIMENSION = 512;
    
    Terrain(int nGridX, int nGridZ, const std::string& strTextureName) noexcept;
    inline auto id() const noexcept { return m_nId; }
    
    inline const auto& vertices() const noexcept { return m_vVertices; }
    inline const auto& texCoords() const noexcept { return m_vTexCoords; }
    inline const auto& indices() const noexcept { return m_vIndices; }
    inline const auto& normals() const noexcept { return m_vNormals; }
    
    inline float worldX() const noexcept { return m_fWorldX; }
    inline float worldZ() const noexcept { return m_fWorldZ; }
    
    inline auto texture() const noexcept { return m_pTexture; }
    
    bool getHeight(float x, float z, float& fOutHeight) const noexcept;
    
    std::pair<int, int> getSelectedTile(const Camera& cam, float fHeightOffset, int nSize) const noexcept;

private:
    
    static std::tuple<float, float, float> getBarycentricCoords(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v) noexcept;
    void generateTerrain() noexcept;
    std::pair<int, int> getTerrainTileFromRay(const Ray&, float fHeightOffset = 0.0f, float fRayLength = 300.0f, int nRecurseLevel = 0) const noexcept;
};

}
 
 */
#endif /* Terrain_h */
