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

namespace Ice
{
    class TerrainSystem : public EntityComponentSystem<TerrainComponent> {
        using triangle_t = Triangle;
        using tree_t = Octree<triangle_t>;

        std::unordered_map<Entity, tree_t> m_mOctrees;

    public:
        struct IntersectResult {
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
        Triangle getTriangleAt(TerrainComponent const& t, float x, float z, float *pfRelX = nullptr, float *pfRelZ = nullptr) const;
        float heightAtHeightMap(TerrainComponent const& terrain, int x, int z) const noexcept;
        glm::vec2 getTerrainLocalCoords(TerrainComponent const& terrain, float x, float z) const;
        glm::vec2 getTriangleRelativeCoords(TerrainComponent const& terrain, float x, float z) const;
        IntersectResult intersects(Entity e, const Ray& ray) const noexcept;

    private:
 };
    
} // namespace Ice


#endif