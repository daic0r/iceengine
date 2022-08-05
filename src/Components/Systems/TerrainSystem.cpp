#include <Components/Systems/TerrainSystem.h>

namespace Ice
{
    std::optional<float> TerrainSystem::getHeight(float x, float z) {
        for (auto e: entities(entityManager.currentScene())) {
            const auto& terrain = entityManager.getComponent<TerrainComponent>(e);

            if (x > terrain.m_terrain.gridX() * terrain.m_terrain.width() &&  x < (terrain.m_terrain.gridX() + 1)*terrain.m_terrain.width() &&
                z > terrain.m_terrain.gridZ() * terrain.m_terrain.height() &&  z < (terrain.m_terrain.gridZ() + 1)*terrain.m_terrain.height()) {

                return terrain.m_terrain.getHeight(x - (terrain.m_terrain.gridX() * terrain.m_terrain.width()), z - (terrain.m_terrain.gridZ() * terrain.m_terrain.height()));
            }
        }
        return std::nullopt;
    }
} // namespace Ice
