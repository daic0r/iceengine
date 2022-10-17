#ifndef LowPolyMeshGenerator_h
#define LowPolyMeshGenerator_h

#include <cstddef>
#include <array>
#include <vector>
#include <System/Math.h>
#include <glm/vec3.hpp>
#include <Utils/MeshGeneration/LowPolyTerrainIndexGenerator.h>
#include <System/RGBA.h>

namespace Ice::MeshGeneration
{
    template<typename IndexGenerator = LowPolyTerrainIndexGenerator>
    class LowPolyTerrainMeshGenerator {
        constexpr static inline auto VERTEX_DIM = 3;
        constexpr static inline auto COLOR_DIM = 4;

        using VertexContType = std::vector<glm::vec3>;//, indexGenerator().getNumVertices() * VERTEX_DIM>;
        using NormalsContType = std::vector<glm::vec3>;//, indexGenerator().getNumVertices() * VERTEX_DIM>;
        using ColorsContType = std::vector<glm::vec4>;//, indexGenerator().getNumVertices() * COLOR_DIM>;
        using IndexContType = IndexGeneratorContainerType; 

        std::size_t Width, Height;
        IndexGenerator m_indexGen;

    public:
        using index_generator_t = IndexGenerator;
        constexpr LowPolyTerrainMeshGenerator(std::size_t nWidth, std::size_t nHeight)
            : Width{ nWidth }, Height{ nHeight }, m_indexGen{ Width, Height } {}

        template<std::size_t NumColors>
        constexpr auto generateVerticesAndColors(
            float fTileWidth,
            float fTileHeight,
            float fHeightRangeMin,
            float fHeightRangeMax,
            const std::vector<float>& arHeightMap,
            const std::array<RGBA, NumColors>& arColors
        ) {
            VertexContType retVertices = generateVertices(fTileWidth, fTileHeight, &arHeightMap);
            ColorsContType retColors{};
            retColors.resize(indexGenerator().getNumVertices());// * COLOR_DIM);
            std::size_t idx{}, colorIdx{};

            for (std::size_t z = 0; z < Height; ++z) {
                const auto color = getVertexColor(arHeightMap[z * Width], fHeightRangeMin, fHeightRangeMax, arColors);
                storeColor(color, retColors, colorIdx);

                for (std::size_t x = 1; x < Width - 1; ++x) {
                    const auto storeColorImpl = [&]() {
                        const auto fHeight = arHeightMap[z * Width + x];
                        const auto color = getVertexColor(fHeight, fHeightRangeMin, fHeightRangeMax, arColors);
                        storeColor(color, retColors, colorIdx);
                    };
                    storeColorImpl();
                    // Don't double vertex in the last 2 rows
                    if (z < Height - 2) {
                        storeColorImpl();
                    }
                }
                const auto color2 = getVertexColor(arHeightMap[z * Width + (Width - 1)], fHeightRangeMin, fHeightRangeMax, arColors);
                storeColor(color2, retColors, colorIdx);
            }
            return std::make_pair(retVertices, retColors);
        }

        constexpr auto generateVertices(
            float fTileWidth,
            float fTileHeight,
            const std::vector<float>* arHeightMap
        )
        {
            VertexContType retVertices{};
            retVertices.resize(indexGenerator().getNumVertices());// * VERTEX_DIM);
            std::size_t idx{};

            for (std::size_t z = 0; z < Height; ++z) {
                storeVertex(0.0f, arHeightMap ? (*arHeightMap)[z * Width] : 0.0f, z * fTileHeight, retVertices, idx);

                for (std::size_t x = 1; x < Width - 1; ++x) {
                    const auto storeVertexImpl = [&]() {
                        const auto fHeight = arHeightMap ? (*arHeightMap)[z * Width + x] : 0.0f;
                        storeVertex(x * fTileWidth, fHeight, z * fTileHeight, retVertices, idx);
                    };
                    storeVertexImpl();
                    // Don't double vertex in the last 2 rows
                    if (z < Height - 2) {
                        storeVertexImpl();
                    }
                }
                storeVertex((Width - 1) * fTileWidth, arHeightMap ? (*arHeightMap)[z * Width + (Width - 1)] : 0.0f, z * fTileHeight, retVertices, idx);
            }
            return retVertices;
        }

        constexpr auto generateNormals(const VertexContType& arVertices, const IndexContType& arIndices) {
            struct vec3 {
                float x, y, z;
                
                constexpr vec3 operator-(const vec3& rhs) const noexcept {
                    return vec3{ x - rhs.x, y - rhs.y, z - rhs.z };
                }

                constexpr vec3 cross(const vec3& rhs) const noexcept {
                    return vec3{ 
                        y * rhs.z - z * rhs.y,
                        z * rhs.x - x * rhs.z,
                        x * rhs.y - y * rhs.x
                    };
                }

                constexpr float length() const noexcept {
                    return Ice::Math::sqrt(x*x + y*y + z*z);
                }

                constexpr vec3 unit() const noexcept {
                    const auto len = length();
                    return vec3{ x / len, y / len, z / len };
                }
            };
            NormalsContType arNormals{};
            arNormals.resize(indexGenerator().getNumVertices());// * VERTEX_DIM);
            for (std::size_t i = 0; i < arIndices.size() - 2; i += 3) {
                const auto vertIndex0 = arIndices[i];
                const auto vertIndex1 = arIndices[i+1];
                const auto vertIndex2 = arIndices[i+2];
                const vec3 p0{ arVertices[vertIndex0].x, arVertices[vertIndex0].y, arVertices[vertIndex0].z };
                const vec3 p1{ arVertices[vertIndex1].x, arVertices[vertIndex1].y, arVertices[vertIndex1].z };
                const vec3 p2{ arVertices[vertIndex2].x, arVertices[vertIndex2].y, arVertices[vertIndex2].z };
                //const vec3 p0{ arVertices[VERTEX_DIM*vertIndex0], arVertices[VERTEX_DIM*vertIndex0+1], arVertices[VERTEX_DIM*vertIndex0+2] };
                //const vec3 p1{ arVertices[VERTEX_DIM*vertIndex1], arVertices[VERTEX_DIM*vertIndex1+1], arVertices[VERTEX_DIM*vertIndex1+2] };
                //const vec3 p2{ arVertices[VERTEX_DIM*vertIndex2], arVertices[VERTEX_DIM*vertIndex2+1], arVertices[VERTEX_DIM*vertIndex2+2] };
                const auto norm = (p1 - p0).cross(p2 - p0).unit();
                storeNormal(norm.x, norm.y, norm.z, arNormals, vertIndex0);
                storeNormal(norm.x, norm.y, norm.z, arNormals, vertIndex1);
                storeNormal(norm.x, norm.y, norm.z, arNormals, vertIndex2);
            }
            return arNormals;
        }

        constexpr IndexGenerator& indexGenerator() { return m_indexGen; }
        constexpr auto generateIndices() {
            return indexGenerator().generateIndices();
        }
    private:
        constexpr void storeVertex(float x, float y, float z, VertexContType& arVertices, std::size_t& idx) {
            arVertices.at(idx) = glm::vec3{ x, y, z };
            idx += 1;
        }

        constexpr void storeColor(const std::tuple<float,float,float> color, ColorsContType& arColors, std::size_t& idx) {
            arColors.at(idx) = glm::vec4{ std::get<0>(color), std::get<1>(color), std::get<2>(color), 1.0f };
            idx += 1;
        }

        constexpr void storeNormal(float x, float y, float z, NormalsContType& arNormals, std::size_t idx) {
            arNormals.at(idx) = glm::vec3{ x, y, z };
        }

    public:
        template<std::size_t NumColors>
        constexpr auto getVertexColor(float fHeight, float fHeightRangeMin, float fHeightRangeMax, const std::array<RGBA, NumColors>& arColors) {
            const auto fIntervalSize = 1.0f / (arColors.size() - 1);
            const auto fNormHeight = Ice::Math::map0to1Range(fHeight, fHeightRangeMin, fHeightRangeMax);
            const auto fContainerPos = fNormHeight / fIntervalSize;
            const auto colorContainerIdx = static_cast<std::size_t>(fContainerPos);
            const auto fBlendFactor = Ice::Math::map0to1Range(fNormHeight, colorContainerIdx*fIntervalSize, (colorContainerIdx+1)*fIntervalSize);
            const auto lowerColor = arColors[colorContainerIdx];
            const auto upperColor = arColors[colorContainerIdx+1 < arColors.size() ? colorContainerIdx + 1 : colorContainerIdx];
            return std::make_tuple( 
                Ice::Math::mix((float)lowerColor.r, (float)upperColor.r, fBlendFactor), 
                Ice::Math::mix((float)lowerColor.g, (float)upperColor.g, fBlendFactor),
                Ice::Math::mix((float)lowerColor.b, (float)upperColor.b, fBlendFactor) 
            );
        }

    };
} // namespace Ice


#endif