#ifndef LowPolyTerrainIndexGenerator_h
#define LowPolyTerrainIndexGenerator_h

#include <cstddef>
#include <array>
#include <algorithm>
#include <Utils/MeshGeneration/VertexTraversalStrategy.h>

namespace Ice::MeshGeneration
{
    using IndexGeneratorContainerType = std::vector<unsigned int>;

    // Width and Height refer to the height map dimensions
    // Number of tiles is one less respectively
    template<std::size_t Width, std::size_t Height>
    class LowPolyTerrainIndexGenerator {
        static constexpr std::size_t getNumIndices() {
            return 6 * (Width - 1) * (Height - 1);
        }

    public:
        static constexpr auto generateIndices() {
            IndexGeneratorContainerType arIndices{}; 
            arIndices.resize(getNumIndices());

            std::size_t idx{};
            for (auto z = 0; z < Height - 1; ++z) {
                for (auto x = 0; x < Width - 1; ++x) {
                    push_quad_indices(x, z, arIndices, idx);
                }
            }

            return arIndices;
        }

        static constexpr const auto& getTraversalStrategyForTile(int x, int z) {
            if (z < Height - 2) {
                if (x % 2 == z % 2) {
                    return LowPolyTerrainQuadVariant1UpperPortionStrategy;
                } else {
                    return LowPolyTerrainQuadVariant2UpperPortionStrategy;
                }
            }
            if (x % 2 == z % 2) {
                return LowPolyTerrainQuadVariant1LastRowStrategy;
            }
            return LowPolyTerrainQuadVariant2LastRowStrategy;
        } 

    private:
        static constexpr std::size_t getNumRowVertices(int z) noexcept {
            if (z >= Height - 2) {
                return Width;
            }
            return 2 + 2 * (Width - 2);
        }

        // bLeft specifies which of the duplicated vertices we want: the one
        // belonging to the left or the right quad
        static constexpr std::size_t getVertexIndex(int vertexX, int vertexZ, bool bLeft) noexcept {
            std::size_t nRetIdx{};
            for (auto z = 0; z < vertexZ; ++z) {
                nRetIdx += getNumRowVertices(z);
            }
            for (auto x = 0; x < vertexX; ++x) {
                if (x == 0 || x == Width - 1 || vertexZ >= Height - 2)
                    nRetIdx += 1;
                else
                    nRetIdx += 2;
            }
            if (vertexX > 0 && vertexX < Width - 1 && vertexZ < Height -2 && !bLeft)
                nRetIdx += 1;

            return nRetIdx;
        }

        static constexpr void push_upper_left_triangle(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) noexcept {
            const auto strat = LowPolyTerrainQuadVariant1UpperPortionStrategy;
            arIndices[idx++] = getVertexIndex(x + strat[0].x, z + strat[0].z, false);
            arIndices[idx++] = getVertexIndex(x + strat[1].x, z + strat[1].z, false);
            arIndices[idx++] = getVertexIndex(x + strat[2].x, z + strat[2].z, true);
        }
        static constexpr void push_lower_right_triangle(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) {
            // Last row of tiles has different ordering due to different provoking vertex
            // Must be - 2 because (x,z) are tile coordinates and we have one less tile in each dimension
            // than vertices => the last tile row index is at Height - 2
            const auto strat = (z < Height - 2) ? LowPolyTerrainQuadVariant1UpperPortionStrategy : LowPolyTerrainQuadVariant1LastRowStrategy;
            arIndices[idx++] = getVertexIndex(x + strat[3].x, z + strat[3].z, true);
            arIndices[idx++] = getVertexIndex(x + strat[4].x, z + strat[4].z, false);
            arIndices[idx++] = getVertexIndex(x + strat[5].x, z + strat[5].z, true);
    }
        static constexpr void push_lower_left_triangle(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) noexcept {
            const auto strat = LowPolyTerrainQuadVariant2UpperPortionStrategy;
            arIndices[idx++] = getVertexIndex(x + strat[0].x, z + strat[0].z, false);
            arIndices[idx++] = getVertexIndex(x + strat[1].x, z + strat[1].z, false);
            arIndices[idx++] = getVertexIndex(x + strat[2].x, z + strat[2].z, true);
        }
        static constexpr void push_upper_right_triangle(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) noexcept {
            // Last row of tiles has different ordering due to different provoking vertex
            // Must be - 2 because (x,z) are tile coordinates and we have one less tile in each dimension
            // than vertices => the last tile row index is at Height - 2
            const auto strat = (z < Height - 2) ? LowPolyTerrainQuadVariant2UpperPortionStrategy : LowPolyTerrainQuadVariant2LastRowStrategy;
            arIndices[idx++] = getVertexIndex(x + strat[3].x, z + strat[3].z, true);
            arIndices[idx++] = getVertexIndex(x + strat[4].x, z + strat[4].z, false);
            arIndices[idx++] = getVertexIndex(x + strat[5].x, z + strat[5].z, true);
    }
        // Takes tile coordinates
        static constexpr void push_quad_indices_variant1(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) noexcept {
            push_upper_left_triangle(x, z, arIndices, idx);
            push_lower_right_triangle(x, z, arIndices, idx);
        }
        static constexpr void push_quad_indices_variant2(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) noexcept {
            push_lower_left_triangle(x, z, arIndices, idx);
            push_upper_right_triangle(x, z, arIndices, idx);
        }

        static constexpr void push_quad_indices(int x, int z, IndexGeneratorContainerType& arIndices, std::size_t& idx) noexcept {
            const auto bIsQuadVariant1 = (x % 2) == (z % 2);
            if (bIsQuadVariant1)
                push_quad_indices_variant1(x, z, arIndices, idx);
            else
                push_quad_indices_variant2(x, z, arIndices, idx);
        }
    };
} // namespace Ice


#endif