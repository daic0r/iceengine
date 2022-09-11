#ifndef TerrainSystem_h
#define TerrainSystem_h

#include <optional>
#include <Entities/EntityComponentSystem.h>
#include <Components/TerrainComponent.h>
#include <array>

namespace Ice
{
    class TerrainSystem : public EntityComponentSystem<TerrainComponent> {
    public:
        //std::optional<float> getHeight(float x, float z) const;
        TerrainComponent* getTerrainAt(float x, float z) const;

        std::optional<float> getHeight(float x, float z, glm::mat4* pMatrix = nullptr) const noexcept;
        bool getCenterCoordsForTile(int x, int z, float& outX, float& outZ) const noexcept;
        bool getTriangleAt(float x, float z, std::array<glm::vec3, 3>& vOutTriangles, float *pfRelX = nullptr, float *pfRelZ = nullptr) const noexcept;
        float heightAtHeightMap(LowPolyTerrain const& terrain, int x, int z) const noexcept;
    };
    
} // namespace Ice


#endif