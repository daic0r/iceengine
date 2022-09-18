//
//  AABB.h
//  IceCraft
//
//  Created by Matthias Gruen on 02.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AABB_h
#define AABB_h

#include <glm/vec3.hpp>
#include <System/Extents.h>
#include <glm/mat4x4.hpp>
#include <bitset>

namespace Ice {

class Ray;

class AABB {

    glm::vec3 m_minVertex;
    glm::vec3 m_maxVertex;
    
public:
    struct sRayAABBIntersectResult {
        std::bitset<3> intersectOctant; // x,y,z, 0 -> min, 1 -> max
        float fDist;
        glm::vec3 point;
    };
    
    constexpr AABB() :
                m_minVertex{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() },
                m_maxVertex{ -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() }
    {}

    constexpr AABB(const glm::vec3& minVert, const glm::vec3& maxVert) : m_minVertex{ minVert }, m_maxVertex{ maxVert } {}
    AABB(const Extents3& ext) : AABB{ ext.minPoint, ext.maxPoint } {}
    
    constexpr auto& minVertex() noexcept { return m_minVertex; }
    constexpr const auto& minVertex() const noexcept { return m_minVertex; }

    constexpr auto& maxVertex() noexcept { return m_maxVertex; }
    constexpr const auto& maxVertex() const noexcept { return m_maxVertex; }

    constexpr auto width() const noexcept { return maxVertex().x - minVertex().x; }
    constexpr auto height() const noexcept { return maxVertex().y - minVertex().y; }
    constexpr auto depth() const noexcept { return maxVertex().z - minVertex().z; }
    constexpr auto size(int nDim) const noexcept { return maxVertex()[nDim] - minVertex()[nDim]; }

    float volume() const noexcept;
    constexpr glm::vec3 center() const noexcept {
        return minVertex() + (maxVertex() - minVertex()) / 2.0f;
    }
    
    constexpr bool contains(const glm::vec3& p) const noexcept {
        for (glm::vec3::length_type i = 0; i < glm::vec3::length(); ++i) {
            if (p[i] < minVertex()[i] || p[i] > maxVertex()[i])
                return false;
        }
        return true;
    }
    bool contains(const AABB& other) const noexcept {
        for (const auto& p : other.cornerVertices())
            if (!contains(p))
                return false;
        return true;
    }

    bool intersects(const AABB& other) const noexcept;
    //bool intersects(const Ray& r, float* fpDistance = nullptr) const noexcept;
    bool intersects(const Ray& r, sRayAABBIntersectResult* pResult = nullptr) const noexcept;
    std::array<glm::vec3, 8> cornerVertices(const glm::mat4& transform = glm::mat4{ 1.0f }) const noexcept;

    AABB transform(const glm::mat4& m) const noexcept;

    constexpr static AABB unboundedBox() { 
        return AABB{ 
                glm::vec3{ -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() },
                glm::vec3{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() }
            }; 
    }
};

}

#endif /* AABB_h */
