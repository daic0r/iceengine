#include <Components/Systems/BiomeSystem.h>
#include <Components/Systems/TerrainSystem.h>
#include <Entities/EntityManager.h>
#include <System/Math.h>
#include <map>
#include <glm/vec2.hpp>
#include <System/Triangle.h>

namespace glm {
bool operator<(const glm::ivec2& lhs, const glm::ivec2& rhs) noexcept {
    if (lhs.x != rhs.x)
        return lhs.x < rhs.x;
    return lhs.y < rhs.y;
}
}

namespace Ice
{

    bool BiomeSystem::update(float fDeltaTime) noexcept {
        std::map<glm::ivec2, glm::vec4> mFinalColors;
        for (auto e : entities(entityManager.currentScene())) {
            auto& biome = entityManager.getComponent<BiomeNodeComponent>(e);
            if (biome.state == BiomeNodeComponent::State::STATIC)
                continue;

            const auto& trans = entityManager.getComponent<TransformComponent>(e);

            glm::vec2 pos{ trans.m_transform[3][0], trans.m_transform[3][2] };

            auto& terrainComp = m_pTerrainSystem->getTerrainAt(pos.x, pos.y);
            auto& terrain = terrainComp.m_terrain;
            // Is nullptr before first run of renderer
            if (terrainComp.m_pColorAttrib == nullptr)
                return true;

            
            const auto fEffectiveRadius = biome.power.getPercentage(biome.fRadius);
            auto nRadius = static_cast<int>(fEffectiveRadius);

            // #TODO: CALCULATE "TERRAIN LOCAL" COORDS
            pos = m_pTerrainSystem->getTerrainLocalCoords(terrainComp, pos.x, pos.y);
            const auto nTileX = static_cast<int>(pos.x / terrain.tileWidth());
            const auto nTileZ = static_cast<int>(pos.y / terrain.tileHeight());

            for (auto z = std::max(0, nTileZ - nRadius); z < std::min((int)terrain.heightMapHeight(), nTileZ + nRadius); ++z) {
                for (auto x = std::max(0, nTileX - (nRadius)); x < std::min((int)terrain.heightMapWidth(), nTileX + (nRadius)); ++x) {
                    const auto dist = glm::length(glm::vec2{ x*terrain.tileWidth(), z*terrain.tileHeight() } - pos);
                    if (dist > fEffectiveRadius)
                        continue;

                    const auto& indexGen = terrain.indexGenerator();
                    const auto nIdxLeft = indexGen.getVertexIndex(x, z, true);
                    const auto nIdxRight = indexGen.getVertexIndex(x, z, false);
                    const auto oldLeftIter = mFinalColors.find(glm::ivec2{ x, z });
                    const auto oldLeft = oldLeftIter == mFinalColors.end() ? terrainComp.m_vOriginalColorBuffer.at(nIdxLeft) : oldLeftIter->second;
                    const auto fMixFactor = dist / nRadius;
                    const auto resultColor = Math::mix(oldLeft, glm::vec4{ biome.color }, 1.0f - fMixFactor);
                    terrainComp.m_pColorAttrib->update(nIdxLeft, resultColor); 
                    terrainComp.m_pColorAttrib->update(nIdxRight, resultColor); 
                    mFinalColors.insert_or_assign(glm::ivec2{ x, z }, resultColor);
                }
            }
            
            if (biome.state == BiomeNodeComponent::State::EXPANDING)
                biome.power = Percent{ biome.power + fDeltaTime };
            else
            if (biome.state == BiomeNodeComponent::State::SHRINKING)
                biome.power = Percent{ biome.power - fDeltaTime };
            if (biome.power >= 100.0f || biome.power <= 0.0f || biome.state == BiomeNodeComponent::State::NONE) {
                biome.state = BiomeNodeComponent::State::STATIC;
            }
        }
        return true;
    }


    std::array<std::pair<BiomeType, float>, BiomeSystem::MAX_BIOMES_PER_POINT> BiomeSystem::getBiomesAt(const Triangle& triangle, const std::array<float, 3>& bary) const {
        std::array<std::pair<BiomeType, float>, MAX_BIOMES_PER_POINT> ret;
        std::size_t nSlot{};
        for (auto e : entities(entityManager.currentScene())) {
            const auto& biome = entityManager.getComponent<BiomeNodeComponent>(e);
            const auto& trans = entityManager.getComponent<TransformComponent>(e);
            
            const auto fEffectiveRadius = biome.power.getPercentage(biome.fRadius);

            std::array<float, 3> arVertexInfluence;
            std::size_t nIdx{};
            for (const auto& v : triangle.toArray()) {
                const auto fDist = glm::length( glm::vec3{ trans.m_transform[3][0], trans.m_transform[3][1], trans.m_transform[3][2] } - v);
                const auto fInfluence = 1.0f - Math::clamp(fDist / fEffectiveRadius, 0.0f, 1.0f);
                arVertexInfluence[nIdx++] = fInfluence;
            }
            const auto fEffective = bary[0] * arVertexInfluence[0] + bary[1] * arVertexInfluence[1] + bary[2] * arVertexInfluence[2];
            ret[nSlot++] = std::make_pair(biome.type, fEffective);
            if (nSlot == MAX_BIOMES_PER_POINT)
                break;
        }

        return ret;
 
    }

    std::array<std::pair<BiomeType, float>, BiomeSystem::MAX_BIOMES_PER_POINT> BiomeSystem::getBiomesAt(float x, float z) const {
        std::array<std::pair<BiomeType, float>, MAX_BIOMES_PER_POINT> ret;

        /*
        auto& comp = m_pTerrainSystem->getTerrainAt(x, z);

        std::array<glm::vec3, 3> arTriangle;
        float fRelX, fRelZ;
        arTriangle = m_pTerrainSystem->getTriangleAt(comp, x, z,  &fRelX, &fRelZ);

        const auto localCoord = m_pTerrainSystem->getTerrainLocalCoords(comp, x, z);
        const auto trianglCoord = m_pTerrainSystem->getTriangleRelativeCoords(comp, localCoord.x, localCoord.y);
        const auto bary = Math::getBarycentricCoords(
            glm::vec2{ arTriangle[0].x, arTriangle[0].z }, 
            glm::vec2{ arTriangle[1].x, arTriangle[1].z }, 
            glm::vec2{ arTriangle[2].x, arTriangle[2].z },
            glm::vec2{ x, z }
        );

        std::size_t nSlot{};
        for (auto e : entities(entityManager.currentScene())) {
            const auto& biome = entityManager.getComponent<BiomeNodeComponent>(e);
            const auto& trans = entityManager.getComponent<TransformComponent>(e);
            
            std::array<float, 3> arVertexInfluence;
            std::size_t nIdx{};
            for (const auto& v : arTriangle) {
                const auto fDist = glm::length( glm::vec3{ trans.m_transform[3][0], trans.m_transform[3][1], trans.m_transform[3][2] } - v);
                const auto fInfluence = 1.0f - Math::clamp(fDist / biome.m_fRadius, 0.0f, 1.0f);
                arVertexInfluence[nIdx++] = fInfluence;
            }
            const auto fEffective = std::get<0>(bary) * arVertexInfluence[0] + std::get<1>(bary) * arVertexInfluence[1] + std::get<2>(bary) * arVertexInfluence[2];
            ret[nSlot++] = std::make_pair(biome.m_type, fEffective);
            if (nSlot == MAX_BIOMES_PER_POINT)
                break;
        }

*/
        return ret;
    }

    void BiomeSystem::onSystemsInitialized() noexcept {
        m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
    }

} // namespace Ice
