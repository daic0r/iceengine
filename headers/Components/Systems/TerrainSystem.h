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

        bool hasTerrainAt(float x, float z) const;
        TerrainComponent& getTerrainAt(float x, float z) const;

        float getHeight(float x, float z, glm::mat4* pMatrix = nullptr) const;
        glm::vec2 getCenterCoordsForTile(TerrainComponent const& terrain, int x, int z) const;
        std::array<glm::vec3, 3> getTriangleAt(TerrainComponent const& t, float x, float z, float *pfRelX = nullptr, float *pfRelZ = nullptr) const;
        float heightAtHeightMap(TerrainComponent const& terrain, int x, int z) const noexcept;
        glm::vec2 getTerrainLocalCoords(TerrainComponent const& terrain, float x, float z) const;
        glm::vec2 getTriangleRelativeCoords(TerrainComponent const& terrain, float x, float z) const;
    };
    
} // namespace Ice


#endif