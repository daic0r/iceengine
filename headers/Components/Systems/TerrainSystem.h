#ifndef TerrainSystem_h
#define TerrainSystem_h

#include <optional>
#include <Entities/EntityComponentSystem.h>
#include <Components/TerrainComponent.h>
#include <array>
#include <unordered_map>
#include <array>
#include <System/Octree.h>
#include <System/Triangle.h>
#include <System/Extents.h>

namespace Ice
{
    class WaterRenderingSystem;

    class TerrainSystem : public EntityComponentSystem<TerrainComponent> {
        using triangle_t = Triangle;
        using tree_t = Octree<std::vector<triangle_t>, triangle_t>;

        std::unordered_map<Entity, tree_t> m_mOctrees;
        Extents2 m_worldExtents;

    public:
        struct IntersectResult {
            bool bIntersects{};
            triangle_t triangle;
            glm::vec3 point;
            std::array<float, 3> barycentric;
        };
        void onEntityAdded(Entity) noexcept;
        void onSystemsInitialized() noexcept override;
        
        bool hasTerrainAt(float x, float z) const;
        TerrainComponent& getTerrainAt(float x, float z) const;

        float getHeight(float x, float z, glm::mat4* pMatrix = nullptr) const;
        glm::vec2 getCenterCoordsForTile(TerrainComponent const& terrain, int x, int z) const;
        Triangle getTriangleAt(TerrainComponent const& t, float x, float z, float *pfRelX = nullptr, float *pfRelZ = nullptr) const;
        float heightAtHeightMap(TerrainComponent const& terrain, int x, int z) const noexcept;
        glm::vec2 getTerrainLocalCoords(TerrainComponent const& terrain, float x, float z) const;
        glm::vec2 getTriangleRelativeCoords(TerrainComponent const& terrain, float x, float z) const;
        IntersectResult intersects(Entity e, const Ray& ray) const noexcept;

        std::vector<glm::vec2> findPath(float x1, float z1, float x2, float z2) const;

        const auto& worldExtents() const noexcept { return m_worldExtents; }

    private:

        WaterRenderingSystem* m_pWaterSys{};
 };
    
} // namespace Ice


#endif