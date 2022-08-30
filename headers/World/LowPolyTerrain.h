#ifndef LowPolyTerrain_h
#define LowPolyTerrain_h

#include <cstddef>
#include <optional>
#include <Utils/MeshGeneration/LowPolyTerrainIndexGenerator.h>
#include <functional>
#include <glm/mat4x4.hpp>

namespace Ice {

    class LowPolyTerrain {
        static inline int NextId{ 0 };
    public:
        LowPolyTerrain(
            int x,
            int z,
            float fTileWidth,
            float fTileHeight,
            std::size_t nWidthHeightMap,
            std::size_t nHeightHeightMap,
            const std::vector<float>& vHeightMap,
            const std::function<const MeshGeneration::VertexTraversalStrategy&(int,int)>& getTraversalStrategyForTileFunc
        )
            : m_vHeightMap{ vHeightMap },
            m_nHeightMapWidth{ nWidthHeightMap },
            m_nHeightMapHeight{ nHeightHeightMap },
            m_nX{ x },
            m_nZ{ z },
            m_fWidth{ fTileWidth * nWidthHeightMap },
            m_fHeight{ fTileHeight * nHeightHeightMap },
            m_fTileWidth{ fTileWidth },
            m_fTileHeight{ fTileHeight },
            m_getTraversalStrategyForTileFunc{ getTraversalStrategyForTileFunc },
            m_nId{ NextId++ }
        {
        }

        constexpr auto heightMapWidth() const noexcept { return m_nHeightMapWidth; }
        constexpr auto heightMapHeight() const noexcept { return m_nHeightMapHeight; }
        constexpr auto gridX() const noexcept { return m_nX; } 
        constexpr auto gridZ() const noexcept { return m_nZ; } 
        constexpr auto width() const noexcept { return m_fWidth; }
        constexpr auto height() const noexcept { return m_fHeight; }
        constexpr auto tileWidth() const noexcept { return m_fTileWidth; }
        constexpr auto tileHeight() const noexcept { return m_fTileHeight; } 
        constexpr decltype(auto) heightMap() const noexcept { return (m_vHeightMap); }

        std::optional<float> getHeight(float x, float z, glm::mat4* pMatrix = nullptr) const noexcept;
        bool getCenterCoordsForTile(int x, int z, float& outX, float& outZ) const noexcept;

        auto terrainId() const noexcept { return m_nId; }

    private:
        int m_nId{};
        std::vector<float> m_vHeightMap;
        std::size_t m_nHeightMapWidth, m_nHeightMapHeight;

        int m_nX, m_nZ; // Position in the grid of terrains
        float m_fWidth, m_fHeight; // Geometrical dimensions
        float m_fTileWidth, m_fTileHeight; // A tile is the area between adjacent height map points

        // Via this function we provide a way to find out how to traverse the height points in
        // order to calculate the terrain height at a given point
        std::function<const MeshGeneration::VertexTraversalStrategy&(int,int)> m_getTraversalStrategyForTileFunc;
    };

}

#endif