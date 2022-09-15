#ifndef TerrainSystem_h
#define TerrainSystem_h

#include <optional>
#include <Entities/EntityComponentSystem.h>
#include <Components/TerrainComponent.h>
#include <array>
#include <unordered_map>
#include <array>
#include <System/Octree.h>

namespace Ice
{
    class TerrainSystem : public EntityComponentSystem<TerrainComponent> {
        using triangle_t = std::array<glm::vec3, 3>;
        using kd_tree_t = Octree<triangle_t>;

        std::unordered_map<Entity, kd_tree_t> m_mOctrees;

    public:
        struct sIntersectResult {
            bool bIntersects{};
            triangle_t triangle;
            glm::vec3 point;
            std::array<float, 3> barycentric;
        };

        void onEntityAdded(Entity) noexcept;
        
        bool hasTerrainAt(float x, float z) const;
        TerrainComponent& getTerrainAt(float x, float z) const;

        float getHeight(float x, float z, glm::mat4* pMatrix = nullptr) const;
        glm::vec2 getCenterCoordsForTile(TerrainComponent const& terrain, int x, int z) const;
        std::array<glm::vec3, 3> getTriangleAt(TerrainComponent const& t, float x, float z, float *pfRelX = nullptr, float *pfRelZ = nullptr) const;
        float heightAtHeightMap(TerrainComponent const& terrain, int x, int z) const noexcept;
        glm::vec2 getTerrainLocalCoords(TerrainComponent const& terrain, float x, float z) const;
        glm::vec2 getTriangleRelativeCoords(TerrainComponent const& terrain, float x, float z) const;
        const std::vector<TerrainSystem::sIntersectResult>& intersects(Entity e, const Ray& ray) const noexcept;

    private:
        mutable std::vector<TerrainSystem::sIntersectResult> m_vIntersectResults;
 };
    
} // namespace Ice


#endif