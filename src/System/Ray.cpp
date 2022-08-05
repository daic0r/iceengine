//
//  Ray.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 11.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Ray.h>
#include <array>
#include <System/Config.h>
#include <glm/vec2.hpp>

namespace Ice {

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) :
    m_origin{ origin }, m_direction{ direction }
{}

void Ray::setOrigin(const glm::vec3& origin) noexcept {
    m_origin = origin;
}

void Ray::setDirection(const glm::vec3& direction) noexcept {
    m_direction = direction;
}

}
