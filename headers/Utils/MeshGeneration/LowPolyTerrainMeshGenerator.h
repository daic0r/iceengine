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
    template<std::size_t Width, std::size_t Height, typename IndexGenerator = LowPolyTerrainIndexGenerator<Width, Height>>
    class LowPolyTerrainMeshGenerator {
        static constexpr auto getNumVertices() {
            return 
                // all except last 2 rows of height map entries (i.e. last row of tiles):
                //  - single vertex first and last column
                //  - 2 for each height map entry in between
                // last two rows of height map:
                //  - only one vertex per height map entry
                //  - that's because of the last row of tiles, the bottom
                //    vertices can provide the color for the right triangle 
                (2 + (Width - 2) * 2) * (Height - 2)
                + (2 * Width);
        }

        static constexpr auto VERTEX_DIM = 3;
        static constexpr auto COLOR_DIM = 4;

        using VertexContType = std::vector<float>;//, getNumVertices() * VERTEX_DIM>;
        using NormalsContType = std::vector<float>;//, getNumVertices() * VERTEX_DIM>;
        using ColorsContType = std::vector<float>;//, getNumVertices() * COLOR_DIM>;
        using IndexContType = IndexGeneratorContainerType; 

    public:
        template<std::size_t NumColors>
        static constexpr auto generateVerticesAndColors(
            float fTileWidth,
            float fTileHeight,
            float fHeightRangeMin,
            float fHeightRangeMax,
            const std::vector<float>& arHeightMap,
            const std::array<RGBA, NumColors>& arColors
        ) {
            VertexContType retVertices{};
            ColorsContType retColors{};
            retVertices.resize(getNumVertices() * VERTEX_DIM);
            retColors.resize(getNumVertices() * COLOR_DIM);
            std::size_t idx{}, colorIdx{};

            for (std::size_t z = 0; z < Height; ++z) {
                storeVertex(0.0f, arHeightMap[z * Width], z * fTileHeight, retVertices, idx);
                const auto color = getVertexColor(arHeightMap[z * Width], fHeightRangeMin, fHeightRangeMax, arColors);
                storeColor(color, retColors, colorIdx);

                for (std::size_t x = 1; x < Width - 1; ++x) {
                    const auto storeVertexAndColor = [&]() {
                        const auto fHeight = arHeightMap[z * Width + x];
                        storeVertex(x * fTileWidth, fHeight, z * fTileHeight, retVertices, idx);
                        const auto color = getVertexColor(fHeight, fHeightRangeMin, fHeightRangeMax, arColors);
                        storeColor(color, retColors, colorIdx);
                    };
                    storeVertexAndColor();
                    // Don't double vertex in the last 2 rows
                    if (z < Height - 2) {
                        storeVertexAndColor();
                    }
                }
                storeVertex((Width - 1) * fTileWidth, arHeightMap[z * Width + (Width - 1)], z * fTileHeight, retVertices, idx);
                const auto color2 = getVertexColor(arHeightMap[z * Width + (Width - 1)], fHeightRangeMin, fHeightRangeMax, arColors);
                storeColor(color2, retColors, colorIdx);
            }
            return std::make_pair(retVertices, retColors);
        }

        static constexpr auto generateNormals(const VertexContType& arVertices, const IndexContType& arIndices) {
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
            arNormals.resize(getNumVertices() * VERTEX_DIM);
            for (std::size_t i = 0; i < arIndices.size() - 2; i += 3) {
                const auto vertIndex0 = arIndices[i];
                const auto vertIndex1 = arIndices[i+1];
                const auto vertIndex2 = arIndices[i+2];
                const vec3 p0{ arVertices[VERTEX_DIM*vertIndex0], arVertices[VERTEX_DIM*vertIndex0+1], arVertices[VERTEX_DIM*vertIndex0+2] };
                const vec3 p1{ arVertices[VERTEX_DIM*vertIndex1], arVertices[VERTEX_DIM*vertIndex1+1], arVertices[VERTEX_DIM*vertIndex1+2] };
                const vec3 p2{ arVertices[VERTEX_DIM*vertIndex2], arVertices[VERTEX_DIM*vertIndex2+1], arVertices[VERTEX_DIM*vertIndex2+2] };
                const auto norm = (p1 - p0).cross(p2 - p0).unit();
                storeNormal(norm.x, norm.y, norm.z, arNormals, vertIndex0);
                storeNormal(norm.x, norm.y, norm.z, arNormals, vertIndex1);
                storeNormal(norm.x, norm.y, norm.z, arNormals, vertIndex2);
            }
            return arNormals;
        }

        static constexpr auto generateIndices() {
            return IndexGenerator::generateIndices();
        }
    private:
        static constexpr void storeVertex(float x, float y, float z, VertexContType& arVertices, std::size_t& idx) {
            arVertices[idx] = x;
            arVertices[idx+1] = y;
            arVertices[idx+2] = z;
            idx += VERTEX_DIM;
        }

        static constexpr void storeColor(const std::tuple<float,float,float> color, ColorsContType& arColors, std::size_t& idx) {
            arColors[idx] = std::get<0>(color);
            arColors[idx+1] = std::get<1>(color);
            arColors[idx+2] = std::get<2>(color);
            arColors[idx+3] = 1.0f; // alpha
            idx += COLOR_DIM;
        }

        static constexpr void storeNormal(float x, float y, float z, NormalsContType& arNormals, std::size_t idx) {
            arNormals[idx*VERTEX_DIM] = x;
            arNormals[idx*VERTEX_DIM+1] = y;
            arNormals[idx*VERTEX_DIM+2] = z;
        }
    public:
        template<std::size_t NumColors>
        static constexpr auto getVertexColor(float fHeight, float fHeightRangeMin, float fHeightRangeMax, const std::array<RGBA, NumColors>& arColors) {
            const auto fIntervalSize = 1.0f / (arColors.size() - 1);
            const auto fNormHeight = Ice::Math::map0to1Range(fHeight, fHeightRangeMin, fHeightRangeMax);
            const auto fContainerPos = fNormHeight / fIntervalSize;
            const auto colorContainerIdx = static_cast<std::size_t>(fContainerPos);
            const auto fBlendFactor = Ice::Math::map0to1Range(fNormHeight, colorContainerIdx*fIntervalSize, (colorContainerIdx+1)*fIntervalSize);
            const auto lowerColor = arColors[colorContainerIdx];
            const auto upperColor = arColors[colorContainerIdx+1]; 
            return std::make_tuple( 
                Ice::Math::mix((float)lowerColor.r, (float)upperColor.r, fBlendFactor), 
                Ice::Math::mix((float)lowerColor.g, (float)upperColor.g, fBlendFactor),
                Ice::Math::mix((float)lowerColor.b, (float)upperColor.b, fBlendFactor) 
            );
        }

    };
} // namespace Ice


#endif