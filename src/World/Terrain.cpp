//
//  Terrain.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

/*
namespace Deprecated {

#include "Terrain.h"
#include <System/SystemServices.h>
#include <Interfaces/ITextureManager.h>
//#include <System/PerlinNoise.h>
#include <iostream>
#include <glm/mat2x2.hpp>
#include <System/Camera.h>
#include <System/Ray.h>
#include <System/Tools.h>
#include <System/SystemServices.h>
#include <Events/EventQueue.h>
#include <Events/TerrainTileSelectedEventArgs.h>
#include <SDL2/SDL.h>

int Terrain::nCurrentId{ 0 };

Terrain::Terrain(int nGridX, int nGridZ, const std::string& strTextureName) noexcept
: m_strTextureName{ strTextureName }
{
    m_fWorldX = SIZE * nGridX;
    m_fWorldZ = SIZE * nGridZ;
    m_nId = nCurrentId++;
    m_fTileSize = SIZE / (float)NUM_PARTS_PER_DIMENSION;
    generateTerrain();
    if (strTextureName.length() > 0)
        m_pTexture = textureManager->getTextureAtlas(strTextureName);
}

// Generate flat mesh
void Terrain::generateTerrain() noexcept {
    static constexpr float fHeightMapPixelSize = 255.0 / SIZE;
    static constexpr float fInvTileSize = 1.0f / 25.0f;

    auto genNoise = [this](double x, double z) {
        auto fNoise = m_noise.noise(fHeightMapPixelSize * x / 10.0f, fHeightMapPixelSize * z / 10.0f, 0.0f) * 30.0f;
        fNoise -= 15.0f;
        return fNoise;
    };
    float fStep = m_fTileSize;
   
    auto getHeightAtVertex = [this](int x, int z) {
        x = x < 0 ? 0 : x;
        x = x > NUM_PARTS_PER_DIMENSION ? NUM_PARTS_PER_DIMENSION : x;
        z = z < 0 ? 0 : z;
        z = z > NUM_PARTS_PER_DIMENSION ? NUM_PARTS_PER_DIMENSION : z;
        return m_vHeights[z][x];
    };
    
    auto calcNormal = [&getHeightAtVertex](int x, int z) {
        float fHeightL = getHeightAtVertex(x - 1, z);
        float fHeightR = getHeightAtVertex(x + 1, z);
        float fHeightD = getHeightAtVertex(x, z - 1);
        float fHeightU = getHeightAtVertex(x, z + 1);
        glm::vec3 normal = glm::vec3{ fHeightL - fHeightR, 2.0f, fHeightD - fHeightU };
        return glm::normalize(normal);
    };
    
    float fHeight;
    m_vHeights.resize(NUM_PARTS_PER_DIMENSION + 1);
    for (int z = 0; z < NUM_PARTS_PER_DIMENSION + 1; ++z) {
        float fZ = static_cast<float>(z);
        //m_vHeights[z].resize(NUM_PARTS_PER_DIMENSION + 1);
        for (int x = 0; x < NUM_PARTS_PER_DIMENSION + 1; ++x) {
            float fX = static_cast<float>(x);
            fHeight = genNoise(fX * fStep, fZ * fStep);
            m_vHeights[z].emplace_back(fHeight);
        }
    }
    
    auto getIndexForVertex = [](int x, int z) {
        return z * (NUM_PARTS_PER_DIMENSION + 1) + x;
    };
    auto getVertexAt = [fStep, this](int x, int z) {
        return glm::vec3{ x * fStep, m_vHeights[z][x], z * fStep };
    };

    for (int z = 0; z < NUM_PARTS_PER_DIMENSION + 1; ++z) {
        float fZ = static_cast<float>(z);
        for (int x = 0; x < NUM_PARTS_PER_DIMENSION + 1; ++x) {
            float fX = static_cast<float>(x);
            
            m_vVertices.emplace_back(fX * fStep);
            m_vVertices.emplace_back(m_vHeights[z][x]);
            m_vVertices.emplace_back(fZ * fStep);
            m_vTexCoords.emplace_back(x * fInvTileSize);
            m_vTexCoords.emplace_back(z * fInvTileSize);
            glm::vec3 normal = calcNormal(x, z);
            m_vNormals.emplace_back(normal.x);
            m_vNormals.emplace_back(normal.y);
            m_vNormals.emplace_back(normal.z);

//            if (z < NUM_PARTS_PER_DIMENSION && x < NUM_PARTS_PER_DIMENSION) {
//                if (x % 2 == 0) {
//                    m_vIndices.emplace_back(getIndexForVertex(x, z + 1));
//                    m_vIndices.emplace_back(getIndexForVertex(x, z));
//                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z + 1));
//                    m_vIndices.emplace_back(getIndexForVertex(x, z));
//                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z + 1));
//                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z));
//                } else {
//                    m_vIndices.emplace_back(getIndexForVertex(x, z));
//                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z));
//                    m_vIndices.emplace_back(getIndexForVertex(x, z + 1));
//                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z));
//                    m_vIndices.emplace_back(getIndexForVertex(x, z + 1));
//                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z + 1));
//                }
//            }
            if (z < NUM_PARTS_PER_DIMENSION && x < NUM_PARTS_PER_DIMENSION) {
                if (z % 2 == 0) {
                    m_vIndices.emplace_back(getIndexForVertex(x, z));
                    m_vIndices.emplace_back(getIndexForVertex(x, z + 1));
                    m_vIndices.emplace_back(x % 2 == 0 ? getIndexForVertex(x + 1, z) : getIndexForVertex(x + 1, z + 1));
                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z + 1));
                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z));
                    m_vIndices.emplace_back(x % 2 == 0 ? getIndexForVertex(x, z + 1) : getIndexForVertex(x, z));
                } else {
                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z));
                    m_vIndices.emplace_back(getIndexForVertex(x, z));
                    m_vIndices.emplace_back(x % 2 == 0 ? getIndexForVertex(x + 1, z + 1) : getIndexForVertex(x, z + 1));
                    m_vIndices.emplace_back(getIndexForVertex(x, z + 1));
                    m_vIndices.emplace_back(getIndexForVertex(x + 1, z + 1));
                    m_vIndices.emplace_back(x % 2 == 0 ? getIndexForVertex(x, z) : getIndexForVertex(x + 1, z));
                }
            }
        }
    }
    
}

bool Terrain::getHeight(float x, float z, float& fOutHeight) const noexcept {
    if (x < m_fWorldX || x > m_fWorldX + SIZE || z < m_fWorldZ || z > m_fWorldZ + SIZE)
        return false;
    
    x = fmod(x, SIZE);
    z = fmod(z, SIZE);
    
    int nTileX = x / m_fTileSize;
    int nTileZ = z / m_fTileSize;
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

std::tuple<float, float, float> Terrain::getBarycentricCoords(const glm::vec2& v1, const glm::vec2& v2, const glm::vec2& v3, const glm::vec2& v) noexcept {
    glm::mat2 invT{ v1.x - v3.x, v1.y - v3.y, v2.x - v3.x, v2.y - v3.y };
    invT = glm::inverse(invT);
    
    glm::vec2 lambdas = invT * (v - v3);
    auto ret = std::make_tuple(lambdas[0], lambdas[1], 1 - lambdas[0] - lambdas[1]);
    return ret;
}

std::pair<int, int> Terrain::getSelectedTile(const Camera& cam, float fHeightOffset, int nSize) const noexcept {
    Camera c{ cam };
    //c.setPosition(c.position() + (c.lookAt() * -5.0f));
    
    MousePicker picker{ cam.matrix() };
    
    glm::vec3 dir = picker.getMouseRay();
    glm::vec3 origin = cam.position();
    
    Ray r{ origin, dir };
    
    int nTileX, nTileZ;
    std::tie(nTileX, nTileZ) = getTerrainTileFromRay(r, fHeightOffset);
    
    if (nSize > 1) {
        ++nTileX; ++nTileZ;
        
        nTileX -= nSize / 2;
        nTileZ -= nSize / 2;
    }
    
    return std::make_pair(nTileX, nTileZ);
    
//    // Triangle strip coordinates
//    glm::vec2 v1{ static_cast<float>(nTileX) * m_fTileSize, static_cast<float>(nTileZ) * m_fTileSize };
//    glm::vec2 v2{ static_cast<float>(nTileX) * m_fTileSize, static_cast<float>(nTileZ + fSize) * m_fTileSize };
//    glm::vec2 v3{ static_cast<float>(nTileX + fSize) * m_fTileSize, static_cast<float>(nTileZ) * m_fTileSize };
//    glm::vec2 v4{ static_cast<float>(nTileX + fSize) * m_fTileSize, static_cast<float>(nTileZ + fSize) * m_fTileSize };
//
//    std::array<glm::vec3, 4> ret;
//    getHeight(v1.x, v1.y, ret[0][1]);
//    getHeight(v2.x, v2.y, ret[1][1]);
//    getHeight(v3.x, v3.y, ret[2][1]);
//    getHeight(v4.x, v4.y, ret[3][1]);
//    ret[0][0] = v1[0]; ret[0][2] = v1[1];
//    ret[1][0] = v2[0]; ret[1][2] = v2[1];
//    ret[2][0] = v3[0]; ret[2][2] = v3[1];
//    ret[3][0] = v4[0]; ret[3][2] = v4[1];
    
//    return ret;
}

std::pair<int, int> Terrain::getTerrainTileFromRay(const Ray& r, float fHeightOffset, float fRayLength, int nRecurseLevel) const noexcept {
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
