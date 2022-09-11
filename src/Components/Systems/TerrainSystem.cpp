#include <Components/Systems/TerrainSystem.h>
#include <bitset>
#include <glm/gtc/matrix_transform.hpp>

namespace Ice
{
    TerrainComponent* TerrainSystem::getTerrainAt(float x, float z) const {
        for (auto e: entities(entityManager.currentScene())) {
            auto& terrain = entityManager.getComponent<TerrainComponent>(e);

            if (x >= terrain.m_terrain.gridX() * terrain.m_terrain.width() &&  x < (terrain.m_terrain.gridX() + 1)*terrain.m_terrain.width() &&
                z >= terrain.m_terrain.gridZ() * terrain.m_terrain.height() &&  z < (terrain.m_terrain.gridZ() + 1)*terrain.m_terrain.height()) {

                return &terrain;
            }
        }
        return nullptr;
    }

    std::optional<float> TerrainSystem::getHeight(float x, float z, glm::mat4* pMatrix) const noexcept {
        float fRelX, fRelZ;
        std::array<glm::vec3, 3> arTriangle;

        if (!getTriangleAt(x, z, arTriangle, &fRelX, &fRelZ))
            return std::nullopt;

        const auto v1 = glm::vec2{ arTriangle[0].x, arTriangle[0].z };
        const auto v2 = glm::vec2{ arTriangle[1].x, arTriangle[1].z };
        const auto v3 = glm::vec2{ arTriangle[2].x, arTriangle[2].z  };

        auto lambdas = Math::getBarycentricCoords(
            v1,
            v2, 
            v3, 
            glm::vec2{ fRelX, fRelZ }
        );

        const auto fRet = std::get<0>(lambdas) * arTriangle[0].y + std::get<1>(lambdas) * arTriangle[1].y + std::get<2>(lambdas) * arTriangle[2].y;
        if (pMatrix) {
            auto pComp = getTerrainAt(x, z);
            if (!pComp)
                return false;

            const auto& terrain = pComp->m_terrain;

            const auto nTileX = static_cast<int>(x / terrain.tileWidth());
            const auto nTileZ = static_cast<int>(z / terrain.tileHeight());

            auto xAxis = glm::normalize(glm::vec3{ terrain.tileWidth(), heightAtHeightMap(terrain, nTileX + 1, nTileZ) - heightAtHeightMap(terrain, nTileX, nTileZ), 0.0f });
            auto zAxis = glm::normalize(glm::vec3{ 0.0f, heightAtHeightMap(terrain, nTileX, nTileZ + 1) - heightAtHeightMap(terrain, nTileX, nTileZ), terrain.tileHeight() });
            const auto yAxis = glm::cross(zAxis, xAxis);
            auto ret = glm::inverse(    // inverse -> transform INTO terrain tile space
                glm::mat4{ 
                    glm::vec4{ xAxis[0], yAxis[0], zAxis[0], 0.0f },
                    glm::vec4{ xAxis[1], yAxis[1], zAxis[1], 0.0f },
                    glm::vec4{ xAxis[2], yAxis[2], zAxis[2], 0.0f },
                    glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }
                }
            );
            // after object is in in terrain tile space, we can translate it to the
            // relevant world position
            ret = glm::translate(glm::mat4{1.0f}, glm::vec3{ x, fRet, z }) * ret;

            *pMatrix = ret;
        }

        return fRet;
    }

    bool TerrainSystem::getCenterCoordsForTile(int x, int z, float& outX, float& outZ) const noexcept {
        auto pComp = getTerrainAt(x, z);
        if (!pComp)
            return false;

        const auto& terrain = pComp->m_terrain;

       if ((x < 0.0f) || (x >= terrain.width()) || (z < 0.0f) || (z >= terrain.height()))
            return false;

        outX = (x * terrain.tileWidth()) + (terrain.tileWidth() / 2.0f);
        outZ = (z * terrain.tileHeight()) + (terrain.tileHeight() / 2.0f);

        return true;
    }
    
    bool TerrainSystem::getTriangleAt(float x, float z, std::array<glm::vec3, 3>& vOutTriangles, float *fpRelX, float *fpRelZ) const noexcept {
        auto pComp = getTerrainAt(x, z);
        if (!pComp)
            return false;

        const auto& terrain = pComp->m_terrain;

        if ((x < 0.0f) || (x >= terrain.width()) || (z < 0.0f) || (z >= terrain.height()))
            return false;
        
        const auto nTileX = static_cast<int>(x / terrain.tileWidth());
        const auto nTileZ = static_cast<int>(z / terrain.tileHeight());

        float fRelX = x - (nTileX * terrain.tileWidth());
        float fRelZ = z - (nTileZ * terrain.tileHeight());
        if (fpRelX)
            *fpRelX = fRelX;
        if (fpRelZ)
            *fpRelZ = fRelZ;

        const auto& tileStrategy = terrain.indexGenerator().getTraversalStrategyForTile(nTileX, nTileZ);

        std::array<glm::vec3, 3> triangle1; // first three vertices, this will be the upper left (variant1) or lower left (variant2) triangle
        std::bitset<4> bitmask{}; // save which combinations of (x,z) we already have
        for (std::size_t i = 0; i < 3; ++i) {
            triangle1[i] = glm::vec3{ tileStrategy[i].x * terrain.tileWidth(), heightAtHeightMap(terrain, nTileX + tileStrategy[i].x, nTileZ + tileStrategy[i].z), tileStrategy[i].z * terrain.tileHeight() };
            bitmask.set(tileStrategy[i].x | (tileStrategy[i].z << 1), true);
        }
            
        std::size_t diagFromPoint, diagToPoint; // vertex index into triangle array
        for (std::size_t i = 0; i < 3; ++i) {
            bool bDone{};
            for (std::size_t j = i + 1; j < 3; ++j) {
                if (!Math::equal(triangle1[i].x, triangle1[j].x) && !Math::equal(triangle1[i].z, triangle1[j].z)) {
                    diagFromPoint = i;
                    diagToPoint = j;
                    if (triangle1[i].x > triangle1[j].x)
                        std::swap(diagFromPoint, diagToPoint);
                    bDone = true;
                    break;
                }
            }
            if (bDone) break;
        }
        const auto bVariant1 = triangle1[diagFromPoint].z > triangle1[diagToPoint].z;
        glm::vec3 thirdVertex{};
        std::size_t nLastIndex{};
        std::tuple<float,float,float> lambdas;
        // Triangle determined above?        
        if ((bVariant1 && (fRelZ <= (terrain.tileHeight() - fRelX))) || (!bVariant1 && (fRelZ >= fRelX)) ){
            nLastIndex = 3 - (diagFromPoint + diagToPoint);
            thirdVertex = triangle1[nLastIndex];
        } else {
            // Construct other triangle
            for (std::size_t i = 3; i < 6; ++i) {
                if (!bitmask.test(tileStrategy[i].x | (tileStrategy[i].z << 1))) {
                    thirdVertex = glm::vec3{ tileStrategy[i].x * terrain.tileWidth(), heightAtHeightMap(terrain, nTileX + tileStrategy[i].x, nTileZ + tileStrategy[i].z), tileStrategy[i].z * terrain.tileHeight() };
                    break;
                }
            }
        }

        vOutTriangles[0] = triangle1[diagFromPoint];
        vOutTriangles[1] = triangle1[diagToPoint];
        vOutTriangles[2] = thirdVertex;
    
        return true;
    }

    float TerrainSystem::heightAtHeightMap(LowPolyTerrain const& terrain, int x, int z) const noexcept {
        return terrain.m_vHeightMap[terrain.heightMapWidth() * z + x];
    }


} // namespace Ice
