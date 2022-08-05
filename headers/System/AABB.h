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

namespace Ice {

class Ray;

class AABB {

    glm::vec3 m_minVertex;
    glm::vec3 m_maxVertex;
    
public:
    
    AABB(const glm::vec3& minVert, const glm::vec3& maxVert) : m_minVertex{ minVert }, m_maxVertex{ maxVert } {}
    AABB(const Extents3& ext) : AABB{ ext.minPoint, ext.maxPoint } {}
    bool intersects(const AABB& other) const noexcept;
    bool intersects(const Ray& r, float* fpDistance = nullptr) const noexcept;
};

}

#endif /* AABB_h */
