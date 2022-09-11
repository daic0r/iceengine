#include <Components/Systems/TerrainSystem.h>

namespace Ice
{
    std::optional<float> TerrainSystem::getHeight(float x, float z) const {
        auto pComp = getTerrainAt(x, z);
        if (!pComp)
            return std::nullopt;

        return pComp->m_terrain.getHeight(x - (pComp->m_terrain.gridX() * pComp->m_terrain.width()), z - (pComp->m_terrain.gridZ() * pComp->m_terrain.height()));
    }

    TerrainComponent* TerrainSystem::getTerrainAt(float x, float z) const {
        for (auto e: entities(entityManager.currentScene())) {
            auto& terrain = entityManager.getComponent<TerrainComponent>(e);

            if (x > terrain.m_terrain.gridX() * terrain.m_terrain.width() &&  x < (terrain.m_terrain.gridX() + 1)*terrain.m_terrain.width() &&
                z > terrain.m_terrain.gridZ() * terrain.m_terrain.height() &&  z < (terrain.m_terrain.gridZ() + 1)*terrain.m_terrain.height()) {

                return &terrain;
            }
        }
        return nullptr;
    }
} // namespace Ice
