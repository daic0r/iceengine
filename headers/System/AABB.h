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

namespace Ice {

class Ray;

class AABB {

    glm::vec3 m_minVertex;
    glm::vec3 m_maxVertex;
    
public:
    
    AABB() :
                m_minVertex{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() },
                m_maxVertex{ -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() }
    {}

    AABB(const glm::vec3& minVert, const glm::vec3& maxVert) : m_minVertex{ minVert }, m_maxVertex{ maxVert } {}
    AABB(const Extents3& ext) : AABB{ ext.minPoint, ext.maxPoint } {}
    
    auto& minVertex() noexcept { return m_minVertex; }
    const auto& minVertex() const noexcept { return m_minVertex; }

    auto& maxVertex() noexcept { return m_maxVertex; }
    const auto& maxVertex() const noexcept { return m_maxVertex; }

    bool intersects(const AABB& other) const noexcept;
    bool intersects(const Ray& r, float* fpDistance = nullptr) const noexcept;
    std::array<glm::vec3, 8> cornerVertices(const glm::mat4& transform = glm::mat4{ 1.0f }) const noexcept;
};

}

#endif /* AABB_h */
