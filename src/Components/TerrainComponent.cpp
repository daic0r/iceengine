//
//  TerrainComponent.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

/*
#include <Components/TerrainComponent.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat2x2.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <System/Camera.h>
#include <GUI/MousePicker.h>
#include <System/Ray.h>
#include <System/Tools.h>

namespace Ice {

int TerrainComponent::nCurrentId{ 0 };

TerrainComponent::TerrainComponent(int nGridX, int nGridZ, float fSize, int nNumPartsPerDimension) noexcept
{
    m_fSize = fSize;
    m_nNumPartsPerDimension = nNumPartsPerDimension;
    m_fWorldX = m_fSize * nGridX;
    m_fWorldZ = m_fSize * nGridZ;
    m_nId = nCurrentId++;
    m_fTileSize = m_fSize / (float)m_nNumPartsPerDimension;
}

bool TerrainComponent::getHeight(float x, float z, float& fOutHeight) const noexcept {
    if ((x < m_fWorldX) || (x >= m_fWorldX + m_fSize) || (z < m_fWorldZ) || (z >= m_fWorldZ + m_fSize))
        return false;
    
    x = fmod(x, m_fSize);
    z = fmod(z, m_fSize);
    
    int nTileX = x / m_fTileSize;
    int nTileZ = z / m_fTileSize;
    if (nTileX > numTiles() - 1 || nTileZ > numTiles() - 1)
        return false;
    float fRelX = fmod(x, m_fTileSize);
    float fRelZ = fmod(z, m_fTileSize);

    glm::vec2 v1;
    glm::vec2 v2;
    glm::vec2 v3;
    
    float fHeight1, fHeight2, fHeight3;
    
    // Lower left triangle or upper right triangle?
    if ((nTileX + nTileZ) % 2 == 0) {
        if (fRelZ <= 1 - fRelX) {
            v1 = { 0, 1 };
            v2 = { 1, 0 };
            v3 = { 0, 0 };
        }
        else {
            v1 = { 0, 1 };
            v2 = { 1, 0 };
            v3 = { 1, 1 };
        }
    } else {
        if (fRelZ <= fRelX) {
            v1 = { 1, 1 };
            v2 = { 0, 0 };
            v3 = { 1, 0 };
        }
        else {
            v1 = { 1, 1 };
            v2 = { 0, 0 };
            v3 = { 0, 1 };
        }
    }
    fHeight1 = m_vHeights[nTileZ + v1.y][nTileX + v1.x];
    fHeight2 = m_vHeights[nTileZ + v2.y][nTileX + v2.x];
    fHeight3 = m_vHeights[nTileZ + v3.y][nTileX + v3.x];
    v1 *= m_fTileSize;
    v2 *= m_fTileSize;
    v3 *= m_fTileSize;
    auto lambdas = getBarycentricCoords(v1, v2, v3, glm::vec2{ fRelX, fRelZ });
    
    fOutHeight = std::get<0>(lambdas) * fHeight1 + std::get<1>(lambdas) * fHeight2 + std::get<2>(lambdas) * fHeight3;
    
    //std::cout << "Height is " <<fOutHeight << std::endl;
    
    return true;
}

bool TerrainComponent::getCenterCoordsForTile(int x, int z, float& outX, float& outZ) const noexcept {
    const int nGridX = static_cast<int>(m_fWorldX / m_fSize);
    const int nGridZ = static_cast<int>(m_fWorldZ / m_fSize);
    if (x < nGridX || z < nGridZ || x > nGridX+m_nNumPartsPerDimension-1 || z > nGridZ+m_nNumPartsPerDimension-1)
        return false;

    outX = (static_cast<float>(nGridX) * m_fSize) + (static_cast<float>(x) * m_fTileSize) + (m_fTileSize / 2.0f);
    outZ = (static_cast<float>(nGridZ) * m_fSize) + (static_cast<float>(z) * m_fTileSize) + (m_fTileSize / 2.0f);

    return true;
}

std::tuple<float, float, float> TerrainComponent::getBarycentricCoords(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v) noexcept {
    glm::mat2 invT{ v1.x - v3.x, v1.y - v3.y, v2.x - v3.x, v2.y - v3.y };
    invT = glm::inverse(invT);
    
    glm::vec2 lambdas = invT * (v - v3);
    auto ret = std::make_tuple(lambdas[0], lambdas[1], 1 - lambdas[0] - lambdas[1]);
    return ret;
}

std::pair<int, int> TerrainComponent::getSelectedTile(const Camera& cam, float fHeightOffset, int nSize) const noexcept {
    MousePicker picker{ cam.matrix() };
    
    glm::vec3 dir = picker.getMouseRay();
    glm::vec3 origin = cam.position();
    
    Ray r{ origin, dir };
    
    auto [nTileX, nTileZ] = getTerrainTileFromRay(r, fHeightOffset);
    
    if (nSize > 1) {
        ++nTileX; ++nTileZ;
        
        nTileX -= nSize / 2;
        nTileZ -= nSize / 2;
    }
    
    return std::make_pair(nTileX, nTileZ);
}

std::pair<int, int> TerrainComponent::getTerrainTileFromRay(const Ray& r, float fHeightOffset, float fRayLength, int nRecurseLevel) const noexcept {
    glm::vec3 halfPoint = r.origin() + (fRayLength / 2.0f) * r.direction();
    float fHeight;

    if (nRecurseLevel > 50)
        return std::make_pair(-1, -1);
    
    bool bSucc = getHeight(halfPoint.x, halfPoint.z, fHeight);
    //std::cout << "Recurse level: " << nRecurseLevel << ", ray length: " << fRayLength << ", fHeight = " << fHeight << ", halfPoint.y = " << halfPoint.y << std::endl;

    if (bSucc) {
        // If point is below terrain, continue with the ray's half that's ABOVE the terrain (i.e. starting at the original ray's origin)
        if (Tools::fEqual(halfPoint.y, fHeight + fHeightOffset) || fRayLength < 1.0f) {
            return std::make_pair(halfPoint.x / m_fTileSize, halfPoint.z / m_fTileSize);
        } else
        if (halfPoint.y < fHeight + fHeightOffset)
            halfPoint = r.origin();
    } else {
        bSucc = getHeight(r.origin().x, r.origin().z, fHeight);
        if (bSucc)
            halfPoint = r.origin();
    }
        
    Ray newRay{ halfPoint, r.direction() };
    return getTerrainTileFromRay(newRay, fHeightOffset, fRayLength / 2.0f, nRecurseLevel + 1);
}
}

*/