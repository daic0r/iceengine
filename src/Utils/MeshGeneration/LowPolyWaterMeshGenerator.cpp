#include <Utils/MeshGeneration/LowPolyWaterMeshGenerator.h>

namespace Ice
{
    void LowPolyWaterMeshGenerator::generate() noexcept {
        const auto nVertsPerRow = m_nNumTilesH + 1;
        for (std::size_t nTileZ = 0; nTileZ < m_nNumTilesV + 1; ++nTileZ) {
            for (std::size_t nTileX = 0; nTileX < m_nNumTilesH + 1; ++nTileX) {
                m_vVertices.emplace_back(nTileX * m_fTileWidth, nTileZ * m_fTileHeight);
                if (nTileZ < m_nNumTilesV && nTileX < m_nNumTilesH) [[likely]] {
                    m_vIndices.push_back(nVertsPerRow * nTileZ + nTileX);
                    m_vIndices.push_back(nVertsPerRow * (nTileZ + 1) + nTileX);
                    m_vIndices.push_back(nVertsPerRow * nTileZ + nTileX + 1);
                    m_vIndices.push_back(nVertsPerRow * nTileZ + nTileX + 1);
                    m_vIndices.push_back(nVertsPerRow * (nTileZ + 1) + nTileX);
                    m_vIndices.push_back(nVertsPerRow * (nTileZ + 1) + nTileX + 1);
                }
            }
        }
    }
} // namespace Ice
