#ifndef LowPolyWaterMeshGenerator_h
#define LowPolyWaterMeshGenerator_h

#include <vector>
#include <glm/vec2.hpp>

namespace Ice
{
    class LowPolyWaterMeshGenerator {
        float m_fTileWidth{}, m_fTileHeight;
        std::size_t m_nNumTilesH{}, m_nNumTilesV{};
        std::vector<glm::vec2> m_vVertices;
        std::vector<unsigned int> m_vIndices;

    public:
        LowPolyWaterMeshGenerator(std::size_t nNumTilesH, std::size_t nNumTilesV, float fTileWidth, float fTileHeight)
            : m_nNumTilesH{ nNumTilesH },
            m_nNumTilesV{ nNumTilesV },
            m_fTileWidth{ fTileWidth },
            m_fTileHeight{ fTileHeight }
        {
            m_vVertices.reserve((m_nNumTilesH + 1) * (m_nNumTilesV + 1));
            m_vIndices.reserve(6 * m_nNumTilesH * m_nNumTilesV);
        }

        void generate() noexcept;
        const auto& vertices() const noexcept { return m_vVertices; }
        const auto& indices() const noexcept { return m_vIndices; }
    };
} // namespace Ice


#endif