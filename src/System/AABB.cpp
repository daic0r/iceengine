//
//  AABB.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 02.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/AABB.h>
#include <tuple>
#include <array>
#include <vector>
#include <glm/vec3.hpp>
#include <functional>
#include <glm/vec2.hpp>
#include <System/Ray.h>
#include <execution>

namespace Ice {

std::array<glm::vec3, 8> AABB::cornerVertices(const glm::mat4& transform) const noexcept {
    auto ret = std::array<glm::vec3, 8>{ 
        m_minVertex,
        glm::vec3{ m_minVertex.x, m_maxVertex.y, m_minVertex.z },
        glm::vec3{ m_minVertex.x, m_minVertex.y, m_maxVertex.z },
        glm::vec3{ m_minVertex.x, m_maxVertex.y, m_maxVertex.z },

        m_maxVertex,
        glm::vec3{ m_maxVertex.x, m_minVertex.y, m_maxVertex.z },
        glm::vec3{ m_maxVertex.x, m_maxVertex.y, m_minVertex.z },
        glm::vec3{ m_maxVertex.x, m_minVertex.y, m_minVertex.z },
    };
    std::transform(std::execution::par_unseq, ret.begin(), ret.end(), ret.begin(), [&transform](const glm::vec3& p) {
        return glm::vec3{ transform * glm::vec4{ p, 1.0f } };
    });
    return ret;
}

bool AABB::intersects(const AABB &other) const noexcept {
    
    for (int i = 0; i < 3; ++i) {
        const auto bCheck1 = (m_maxVertex[i] >= other.m_minVertex[i] && m_maxVertex[i] <= other.m_maxVertex[i]) || (other.m_maxVertex[i] >= m_minVertex[i] && other.m_maxVertex[i] <= m_maxVertex[i]);
        if (!bCheck1) {
            const auto bCheck2 = (m_minVertex[i] >= other.m_minVertex[i] && m_minVertex[i] <= other.m_maxVertex[i]) || (other.m_minVertex[i] >= m_minVertex[i] && other.m_minVertex[i] <= m_maxVertex[i]);
            if (!bCheck2)
                return false;
        }
    }
    return true;
}

float AABB::volume() const noexcept {
    return (maxVertex().x - minVertex().x) * (maxVertex().y - minVertex().y) * (maxVertex().z - minVertex().z);
}

/*
bool AABB::intersects(const Ray& r, float* fpDistance) const noexcept {
    const auto faces = std::invoke([this]() {
        std::array<std::array<glm::vec3, 4>, 6> _faces;
        std::vector<std::array<size_t, 2>> vOtherDims = { { 1, 2 }, { 0, 2 }, { 0, 1 } };

        for (int dim = 0; dim < 3; ++dim) {
            auto& faceMin = _faces[2 * dim];
            auto& faceMax = _faces[2 * dim + 1];
            for (int i = 0; i < 4; ++i) {
                faceMin[i][dim] = m_minVertex[dim];
                faceMax[i][dim] = m_maxVertex[dim];
            }
            for (int i = 0; i < 2; ++i) {
                const size_t nOtherDim = vOtherDims[dim][i];
                faceMin[0][nOtherDim] = m_minVertex[nOtherDim];
                faceMin[1][nOtherDim] = nOtherDim % 2 == 1 ? m_minVertex[nOtherDim] : m_maxVertex[nOtherDim];
                faceMin[2][nOtherDim] = m_maxVertex[nOtherDim];
                faceMin[3][nOtherDim] = nOtherDim % 2 == 1 ? m_minVertex[nOtherDim] : m_maxVertex[nOtherDim];
            }
        }

        return _faces;
    });

    std::tuple<float, glm::vec3, int> foundPointAtDistance{ std::numeric_limits<float>::max(), glm::vec3{}, -1 };
    int nNumFound{ 0 };
    bool bFound = false;

    // Check all 6 faces

    for (int i = 0; i < 6; ++i) {
        auto nTargetDimension = i / 2; // x, y or z
        auto& face = faces[i];
        const float fTarget = face[0][nTargetDimension]; // get dimension value (they should all be equal for the face)
        float t = (fTarget - r.origin()[nTargetDimension]) / r.direction()[nTargetDimension];
        // no intersection
        if (t <= 0.0f)
            continue;

        // get 2D vectors of the remaining 2 dimensions for bounds check
        glm::vec2 hitPoint, faceMin, faceMax;
        int nIndex = 0;
        for (int j = 0; j < 3; ++j) {
            if (j == nTargetDimension)
                continue;
            hitPoint[nIndex] = r.origin()[j] + r.direction()[j] * t;
            faceMin[nIndex] = face[0][j];
            faceMax[nIndex] = face[2][j];    // 3rd vertex (x, y and z)
            if (faceMin[nIndex] > faceMax[nIndex])
                std::swap(faceMin[nIndex], faceMax[nIndex]);
            ++nIndex;
        }

        if (hitPoint.x >= faceMin.x && hitPoint.x <= faceMax.x && hitPoint.y >= faceMin.y && hitPoint.y <= faceMax.y) {
            if (t < std::get<0>(foundPointAtDistance)) {
                glm::vec3 intersectAt = r.origin() + r.direction() * t;
                foundPointAtDistance = std::make_tuple(t, intersectAt, i);
                bFound = true;
                ++nNumFound;
                if (nNumFound == 2) // max 2 intersections possible
                    break;
            }
        }
    }

    if (fpDistance != nullptr) {
        *fpDistance = std::get<0>(foundPointAtDistance);
    }

    return bFound;
}
*/

// See: https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool AABB::intersects(const Ray& r, sRayAABBIntersectResult* pResult) const noexcept {
    const auto invDir = 1.0f / r.direction();
    
    const std::array<bool, 3> arSgn{ invDir.x < 0.0f, invDir.y < 0.0f, invDir.z < 0.0f };

    const auto getRayDependentMinMax = [&](int nDim) {
        float tmin = ((arSgn[nDim] ? maxVertex()[nDim] : minVertex()[nDim]) - r.origin()[nDim]) * invDir[nDim];
        float tmax = ((!arSgn[nDim] ? maxVertex()[nDim] : minVertex()[nDim]) - r.origin()[nDim]) * invDir[nDim];
        return std::make_pair(tmin, tmax);
    };
    auto [tmin, tmax] = getRayDependentMinMax(0);
    const auto [tymin, tymax] = getRayDependentMinMax(1);

    if (tmin > tymax || tymin > tmax)
        return false;

    if (tymin > tmin)
        tmin = tymin;
    if (tymax < tmax)
        tmax = tymax;

    const auto [tzmin, tzmax] = getRayDependentMinMax(2);

    if (tmin > tzmax || tzmin > tmax)
        return false;

    if (tzmin > tmin)
        tmin = tzmin;
    if (tzmax < tmax)
        tmax = tzmax;

    auto t = tmin;
    if (t < 0.0f) {
        t = tmax;
        if (t < 0.0f) return false;
    }
    if (pResult) {
        pResult->fDist = t;
        pResult->point = r.origin() + t * r.direction();
        const auto cen = center();
        pResult->intersectOctant.set(0, pResult->point.x > cen.x);
        pResult->intersectOctant.set(1, pResult->point.y > cen.y);
        pResult->intersectOctant.set(2, pResult->point.z > cen.z);
    }

    return true;
}

AABB AABB::transform(const glm::mat4& m) const noexcept {
    AABB boxWorld{};
    const auto corners = cornerVertices(m);
    for (const auto& corner : corners) {
        for (int i = 0; i < 3; ++i) {
            if (corner[i] < boxWorld.minVertex()[i]) boxWorld.minVertex()[i] = corner[i];
            if (corner[i] > boxWorld.maxVertex()[i]) boxWorld.maxVertex()[i] = corner[i];
        }
    }
    return boxWorld;
}

}
