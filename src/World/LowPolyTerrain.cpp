#include <World/LowPolyTerrain.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <System/Math.h>
#include <bitset>

namespace Ice
{
    
    std::optional<float> LowPolyTerrain::getHeight(float x, float z) const noexcept {
        if ((x < 0.0f) || (x >= width()) || (z < 0.0f) || (z >= height()))
            return std::nullopt;
        
        const auto nTileX = static_cast<int>(x / tileWidth());
        const auto nTileZ = static_cast<int>(z / tileHeight());

        float fRelX = x - static_cast<int>(x);
        float fRelZ = z - static_cast<int>(z);

        const auto& tileStrategy = m_getTraversalStrategyForTileFunc(nTileX, nTileZ);

        const auto heightAt = [this](int x, int z) {
            return m_vHeightMap[heightMapWidth() * z + x];
        };

        std::array<glm::vec3, 3> triangle1; // first three vertices, this will be the upper left (variant1) or lower left (variant2) triangle
        std::bitset<4> bitmask{}; // save which combinations of (x,z) we already have
        for (std::size_t i = 0; i < 3; ++i) {
            triangle1[i] = glm::vec3{ tileStrategy[i].x * tileWidth(), heightAt(nTileX + tileStrategy[i].x, nTileZ + tileStrategy[i].z), tileStrategy[i].z * tileHeight() };
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
        if ((bVariant1 && fRelZ <= tileHeight() - fRelX) || (!bVariant1 && fRelZ >= fRelX) ){
            nLastIndex = 3 - (diagFromPoint + diagToPoint);
            thirdVertex = triangle1[nLastIndex];
        } else {
            // Construct other triangle
            for (std::size_t i = 3; i < 6; ++i) {
                if (!bitmask.test(tileStrategy[i].x | (tileStrategy[i].z << 1))) {
                    thirdVertex = glm::vec3{ tileStrategy[i].x * tileWidth(), heightAt(nTileX + tileStrategy[i].x, nTileZ + tileStrategy[i].z), tileStrategy[i].z * tileHeight() };
                    break;
                }
            }
        }
        const auto v1 = glm::vec2{ triangle1[diagFromPoint].x, triangle1[diagFromPoint].z };
        const auto v2 = glm::vec2{ triangle1[diagToPoint].x, triangle1[diagToPoint].z };
        const auto v3 = glm::vec2{ thirdVertex.x, thirdVertex.z };

        lambdas = Math::getBarycentricCoords(
            v1,
            v2, 
            v3, 
            glm::vec2{ fRelX, fRelZ }
        );

        return std::get<0>(lambdas) * triangle1[diagFromPoint].y + std::get<1>(lambdas) * triangle1[diagToPoint].y + std::get<2>(lambdas) * thirdVertex.y;
    }

    bool LowPolyTerrain::getCenterCoordsForTile(int x, int z, float& outX, float& outZ) const noexcept {
        /*const int nGridX = static_cast<int>(m_fWorldX / m_fSize);
        const int nGridZ = static_cast<int>(m_fWorldZ / m_fSize);
        if (x < nGridX || z < nGridZ || x > nGridX+m_nNumPartsPerDimension-1 || z > nGridZ+m_nNumPartsPerDimension-1)
            return false;
        */
        if ((x < 0.0f) || (x >= width()) || (z < 0.0f) || (z >= height()))
            return false;

        outX = (x * tileWidth()) + (tileWidth() / 2.0f);
        outZ = (z * tileHeight()) + (tileHeight() / 2.0f);

        return true;
    }
} // namespace Ice
