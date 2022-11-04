#include <Components/Systems/TerrainSystem.h>
#include <bitset>
#include <glm/gtc/matrix_transform.hpp>
#include <Components/MeshComponent.h>
#include <glm/gtx/string_cast.hpp>
#include <Interfaces/ITerrainRenderer.h>
#include <Algorithms/AStar.h>
#include <Components/Systems/WaterRenderingSystem.h>

namespace Ice
{
    void TerrainSystem::onSystemsInitialized() noexcept {
        m_pWaterSys = entityManager.getSystem<WaterRenderingSystem, true>();
    }

    void TerrainSystem::onEntityAdded(Entity e) noexcept {
        const auto& mesh = entityManager.getComponent<MeshComponent>(e);
        const auto& terrain = entityManager.getComponent<TerrainComponent>(e);
        m_worldExtents.extend(glm::vec2{ terrain.m_terrain.gridX() * terrain.m_terrain.width(),  terrain.m_terrain.gridZ() * terrain.m_terrain.height() });
        m_worldExtents.extend(glm::vec2{ terrain.m_terrain.gridX() * terrain.m_terrain.width() + terrain.m_terrain.width(),  terrain.m_terrain.gridZ() * terrain.m_terrain.height() + terrain.m_terrain.height() });

        std::vector<std::pair<AABB, triangle_t>> vTriangles;
        AABB outerBox;
        for (std::size_t i = 0, nCount = mesh.indices().size() / 3; i < mesh.indices().size(); i+=3) { // 3 indices at a time => 1 triangle
            triangle_t triangle;
            AABB triangleBox;
            for (std::size_t j = 0; j < 3; ++j) {
                triangle[j] = mesh.vertices().at(mesh.indices().at(i+j));
                for (std::size_t k = 0; k < 3; ++k) {
                    if (triangle[j][k] < triangleBox.minVertex()[k])
                        triangleBox.minVertex()[k] = triangle[j][k];
                    if (triangle[j][k] > triangleBox.maxVertex()[k])
                        triangleBox.maxVertex()[k] = triangle[j][k];

                     if (triangle[j][k] < outerBox.minVertex()[k])
                        outerBox.minVertex()[k] = triangle[j][k];
                    if (triangle[j][k] > outerBox.maxVertex()[k])
                        outerBox.maxVertex()[k] = triangle[j][k];
                }
            }
            vTriangles.emplace_back(triangleBox, triangle);
        }
        //const auto fAddHeight = (outerBox.width() - outerBox.height()) / 2.0f;
        //outerBox.minVertex().y -= fAddHeight;
        //outerBox.maxVertex().y += fAddHeight;
        //auto [iter, _] = m_mOctrees.emplace(std::piecewise_construct, std::forward_as_tuple(e), std::forward_as_tuple(vTriangles, outerBox));
        auto [iter, _] = m_mOctrees.emplace(e, tree_t{});
        iter->second.setEmplaceFunc([](std::vector<triangle_t>& vContainer, const triangle_t& triangle) {
            vContainer.push_back(triangle);
        });
        iter->second.construct(outerBox, &vTriangles);
 #ifdef _DEBUG_OCTREE
        for (auto info = iter->second.startTraversal(); info.box != nullptr; info = iter->second.next()) {
            const auto &[nID, pBB, bIsLeaf] = info; 
            std::cout << "<" << glm::to_string(pBB->minVertex()) << " - " << glm::to_string(pBB->maxVertex()) << ">\n";
        }
        auto pTerrainRenderer = systemServices.getTerrainRenderer();
        pTerrainRenderer->setOctree(&iter->second);
#endif
    }

    TerrainSystem::IntersectResult TerrainSystem::intersects(Entity e, const Ray& ray) const noexcept {
        const auto& tree = m_mOctrees.at(e);
        IntersectResult res;
        tree.intersects(ray, [this,&ray,&res](const std::vector<triangle_t>& vOut) {
            for (const auto& triangle : vOut) {
               if (const auto intersect = triangle.intersects(ray); intersect.bIntersects) {
                    res.bIntersects = true;
                    res.triangle = triangle;
                    res.point = intersect.point;
                    res.barycentric = intersect.barycentric;
                    return SubdivisionIntersectionBehavior::ABORT_SUCCESS;
                }
            }
            return SubdivisionIntersectionBehavior::CONTINUE;
        });
        return res;
    }

    bool TerrainSystem::hasTerrainAt(float x, float z) const {
        for (auto e: entities(entityManager.currentScene())) {
            auto& terrain = entityManager.getComponent<TerrainComponent>(e);

            if (x >= terrain.m_terrain.gridX() * terrain.m_terrain.width() &&  x < (terrain.m_terrain.gridX() + 1)*terrain.m_terrain.width() &&
                z >= terrain.m_terrain.gridZ() * terrain.m_terrain.height() &&  z < (terrain.m_terrain.gridZ() + 1)*terrain.m_terrain.height()) {

                return true;
            }
        }
        return false;
    }

    TerrainComponent& TerrainSystem::getTerrainAt(float x, float z) const {
        for (auto e: entities(entityManager.currentScene())) {
            auto& terrain = entityManager.getComponent<TerrainComponent>(e);

            if (x >= terrain.m_terrain.gridX() * terrain.m_terrain.width() &&  x < (terrain.m_terrain.gridX() + 1)*terrain.m_terrain.width() &&
                z >= terrain.m_terrain.gridZ() * terrain.m_terrain.height() &&  z < (terrain.m_terrain.gridZ() + 1)*terrain.m_terrain.height()) {

                return terrain;
            }
        }
        throw std::runtime_error("No terrain at given coordinates");
    }

    float TerrainSystem::getHeight(float x, float z, glm::mat4* pMatrix) const {
        float fRelX, fRelZ;

        const auto &comp = getTerrainAt(x, z);

        const auto arTriangle = getTriangleAt(comp, x, z, &fRelX, &fRelZ);

        const auto v1 = glm::vec2{ arTriangle[0].x, arTriangle[0].z };
        const auto v2 = glm::vec2{ arTriangle[1].x, arTriangle[1].z };
        const auto v3 = glm::vec2{ arTriangle[2].x, arTriangle[2].z  };

        auto lambdas = Math::getBarycentricCoords(
            v1,
            v2, 
            v3, 
            glm::vec2{ x, z }
        );

        const auto fRet = lambdas[0] * arTriangle[0].y + lambdas[1] * arTriangle[1].y + lambdas[2] * arTriangle[2].y;
        if (pMatrix) {
            const auto& terrain = comp.m_terrain;

            const auto nTileX = static_cast<int>(x / terrain.tileWidth());
            const auto nTileZ = static_cast<int>(z / terrain.tileHeight());

            auto xAxis = glm::normalize(glm::vec3{ terrain.tileWidth(), heightAtHeightMap(comp, nTileX + 1, nTileZ) - heightAtHeightMap(comp, nTileX, nTileZ), 0.0f });
            auto zAxis = glm::normalize(glm::vec3{ 0.0f, heightAtHeightMap(comp, nTileX, nTileZ + 1) - heightAtHeightMap(comp, nTileX, nTileZ), terrain.tileHeight() });
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

    glm::vec2 TerrainSystem::getCenterCoordsForTile(TerrainComponent const& terrain, int x, int z) const {
        const auto& t = terrain.m_terrain;

        if ((x < 0) || (x > (t.width() / t.tileWidth()) - 1) || (z < 0) || (z > (t.height() - t.tileHeight()) - 1))
            throw std::runtime_error("invalid tile coordinates");

        glm::vec2 ret{};
        ret.x = (x * t.tileWidth()) + (t.tileWidth() / 2.0f);
        ret.y = (z * t.tileHeight()) + (t.tileHeight() / 2.0f);

        return ret;
    }
    
    Triangle TerrainSystem::getTriangleAt(TerrainComponent const& t, float x, float z, float *fpRelX, float *fpRelZ) const {
        const auto& terrain = t.m_terrain;

        glm::vec2 ret = getTerrainLocalCoords(t, x, z);

        x = ret.x;
        z = ret.y;

        const auto nTileX = static_cast<int>(x / terrain.tileWidth());
        const auto nTileZ = static_cast<int>(z / terrain.tileHeight());

        float fRelX = x - (nTileX * terrain.tileHeight());
        float fRelZ = z - (nTileZ * terrain.tileHeight());
        if (fpRelX)
            *fpRelX = fRelX;
        if (fpRelZ)
            *fpRelZ = fRelZ;

        const auto& tileStrategy = terrain.indexGenerator().getTraversalStrategyForTile(nTileX, nTileZ);

        std::array<glm::vec3, 3> triangle1; // first three vertices, this will be the upper left (variant1) or lower left (variant2) triangle
        std::bitset<4> bitmask{}; // save which combinations of (x,z) we already have
        for (std::size_t i = 0; i < 3; ++i) {
            triangle1[i] = glm::vec3{ tileStrategy[i].x * terrain.tileWidth(), heightAtHeightMap(t, nTileX + tileStrategy[i].x, nTileZ + tileStrategy[i].z), tileStrategy[i].z * terrain.tileHeight() };
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
                    thirdVertex = glm::vec3{ tileStrategy[i].x * terrain.tileWidth(), heightAtHeightMap(t, nTileX + tileStrategy[i].x, nTileZ + tileStrategy[i].z), tileStrategy[i].z * terrain.tileHeight() };
                    break;
                }
            }
        }

        Triangle triangle;
        const auto vAdd = glm::vec3{ (nTileX*terrain.tileWidth())+(terrain.gridX() * terrain.width()), 0, (nTileZ *terrain.tileHeight())+(terrain.gridZ() * terrain.height()) };
        triangle[0] = triangle1[diagFromPoint] + vAdd;
        triangle[1] = triangle1[diagToPoint] + vAdd;
        triangle[2] = thirdVertex + vAdd;
    
        return triangle;
    }

    float TerrainSystem::heightAtHeightMap(TerrainComponent const& terrain, int x, int z) const noexcept {
        return terrain.m_terrain.m_vHeightMap[terrain.m_terrain.heightMapWidth() * z + x];
    }

    glm::vec2 TerrainSystem::getTerrainLocalCoords(TerrainComponent const& terrain, float x, float z) const {
        return glm::vec2{ fmod(x, terrain.m_terrain.width()), fmod(z, terrain.m_terrain.height()) };
    }

    glm::vec2 TerrainSystem::getTriangleRelativeCoords(TerrainComponent const& terrain, float x, float z) const {
        if ((x < 0) || (x >= terrain.m_terrain.width()) || (z < 0) || (z >= terrain.m_terrain.height()))
            throw std::runtime_error("Invalid terrain-local coordinates");

        const auto nTileX = static_cast<int>(x / terrain.m_terrain.tileWidth());
        const auto nTileZ = static_cast<int>(z / terrain.m_terrain.tileHeight());
        return glm::vec2{ x - nTileX*terrain.m_terrain.tileWidth(), z - nTileZ*terrain.m_terrain.tileHeight() };

    }

    std::vector<glm::vec2> TerrainSystem::findPath(float x1, float z1, float x2, float z2) const {
        const auto& terr1 = getTerrainAt(x1, z1);
        const auto& terr2 = getTerrainAt(x2, z2);

        assert(terr1.m_terrain.tileWidth() == terr2.m_terrain.tileWidth() && terr1.m_terrain.tileHeight() == terr2.m_terrain.tileHeight());

        const auto fTileWidth = terr1.m_terrain.tileWidth() / 3.0f;
        const auto fTileHeight = terr1.m_terrain.tileHeight() / 3.0f;

        const auto nDiffGridX = Math::abs(terr1.m_terrain.gridX() - terr2.m_terrain.gridX());
        const auto nDiffGridZ = Math::abs(terr1.m_terrain.gridZ() - terr2.m_terrain.gridZ());
        const auto nNumTilesW = static_cast<int>(terr1.m_terrain.width() / fTileWidth);
        const auto nNumTilesH = static_cast<int>(terr1.m_terrain.height() / fTileHeight);
        const auto nGridWidth = (nDiffGridX + 1) * nNumTilesW;
        const auto nGridHeight = (nDiffGridZ + 1) * nNumTilesH;

        AStar star(nGridWidth, nGridHeight);

        const auto nTileX1 = static_cast<int>(x1 / fTileWidth) + (terr1.m_terrain.gridX() < terr2.m_terrain.gridX() ? 0 : nDiffGridX) * nNumTilesW; 
        const auto nTileZ1 = static_cast<int>(z1 / fTileHeight) + (terr1.m_terrain.gridZ() < terr2.m_terrain.gridZ() ? 0 : nDiffGridZ) * nNumTilesH; 
        const auto nTileX2 = static_cast<int>(x2 / fTileWidth) + (terr2.m_terrain.gridX() < terr1.m_terrain.gridX() ? 0 : nDiffGridX) * nNumTilesW; 
        const auto nTileZ2 = static_cast<int>(z2 / fTileHeight) + (terr2.m_terrain.gridZ() < terr1.m_terrain.gridZ() ? 0 : nDiffGridZ) * nNumTilesH; 

        std::pair<int,int> from = std::make_pair(nTileX1, nTileZ1);
        std::pair<int,int> to = std::make_pair(nTileX2, nTileZ2);


        //std::pair<int,int> from = std::make_pair((int) (x1 / 15.0f), (int) (curPos.y / 15.0f));
        //std::pair<int,int> to = std::make_pair((int) (walk.target.x / 15.0f), (int) (walk.target.y / 15.0f));
        auto vPath = star.findPath(from, to, 
            [&,this](const auto& nodeCoord) { 
                const auto pos = glm::vec2{ fTileWidth * nodeCoord.first, fTileHeight * nodeCoord.second };
                const auto fHeightFrom = getHeight(pos.x, pos.y);
                if (fHeightFrom <= m_pWaterSys->waterLevel())
                    return std::numeric_limits<float>::max();
                const auto fHeightTo = getHeight(x2, z2);
                return glm::length(glm::vec3{ pos.x, fHeightFrom, pos.y } - glm::vec3{ x2, fHeightTo, z2 });
            },
            [&,this](const auto& a, const auto& b) {
                const auto worldA = glm::vec2{ fTileWidth * a.first, fTileHeight * a.second };
                const auto worldB = glm::vec2{ fTileWidth * b.first, fTileHeight * b.second };
                const auto fHeightA = getHeight(worldA.x, worldA.y);
                const auto fHeightB = getHeight(worldB.x, worldB.y);
                glm::vec3 p1 { worldA.x, fHeightA, worldA.y };
                glm::vec3 p2 { worldB.x, fHeightB, worldB.y };
                return glm::length(p1 - p2);
            }
        );
        std::vector<glm::vec2> vRet;
        vRet.reserve(vPath.size() + 1);
        vRet.push_back(glm::vec2{ x2, z2 });
        for (const auto& rb : vPath) {
            vRet.emplace_back(fTileWidth * rb.first, fTileHeight * rb.second);
        }

        return vRet;        
    }
} // namespace Ice
