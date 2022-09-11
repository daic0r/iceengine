#include <Components/Systems/BiomeSystem.h>
#include <Components/Systems/TerrainSystem.h>
#include <Entities/EntityManager.h>
#include <System/Math.h>

namespace Ice
{
    bool BiomeSystem::update(float fDeltaTime) noexcept {
        static bool done = false;
        if (done) return true;
        for (auto e : entities(entityManager.currentScene())) {
            const auto& biome = entityManager.getComponent<BiomeNodeComponent>(e);
            const auto& trans = entityManager.getComponent<TransformComponent>(e);

            glm::vec2 pos{ trans.m_transform[3][0], trans.m_transform[3][2] };

            auto& terrain = m_pTerrainSystem->getTerrainAt(pos.x, pos.y)->m_terrain;
            if (terrain.colorVertexAttribute() == nullptr)
                return true;

            auto nRadius = 10; // in "height map entries", i.e. discrete nodes in the grid

            // #TODO: CALCULATE "TERRAIN LOCAL" COORDS
            const auto nTileX = static_cast<int>(pos.x / terrain.tileWidth());
            const auto nTileZ = static_cast<int>(pos.y / terrain.tileHeight());


            for (auto z = std::max(0, nTileZ - (nRadius / 2)); z < std::min((int)terrain.heightMapHeight(), nTileZ + (nRadius / 2)); ++z) {
                for (auto x = std::max(0, nTileX - (nRadius / 2)); x < std::min((int)terrain.heightMapWidth(), nTileX + (nRadius / 2)); ++x) {
                    const auto dist = glm::length(glm::vec2{ x, z } - glm::vec2{ nTileX, nTileZ } );
                    if (dist > nRadius)
                        continue;

                    const auto& indexGen = terrain.indexGenerator();
                    const auto nIdxLeft = indexGen.getVertexIndex(x, z, true);
                    const auto nIdxRight = indexGen.getVertexIndex(x, z, false);
                    const auto oldLeft = terrain.colorVertexAttribute()->at<glm::vec4>(nIdxLeft);
                    const auto oldRight = terrain.colorVertexAttribute()->at<glm::vec4>(nIdxRight);
                    const auto fMixFactor = dist / nRadius;
                    const auto resultColorLeft = Math::mix(oldLeft, glm::vec4{ biome.m_color }, 1.0f - fMixFactor);
                    const auto resultColorRight = Math::mix(oldRight, glm::vec4{ biome.m_color }, 1.0f - fMixFactor);
                    terrain.colorVertexAttribute()->update(nIdxLeft, resultColorLeft); 
                    terrain.colorVertexAttribute()->update(nIdxRight, resultColorRight); 
                }
            }
            done = true;
        }
        return true;
    }


    void BiomeSystem::onSystemsInitialized() noexcept {
        m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
    }

} // namespace Ice
