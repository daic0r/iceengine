//
//  Ray.h
//  IceCraft
//
//  Created by Matthias Gruen on 11.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Ray_h
#define Ray_h

#include <glm/vec3.hpp>

namespace Ice {

class Ray {
    glm::vec3 m_origin;
    glm::vec3 m_direction;

public:

    Ray(const glm::vec3&, const glm::vec3&);
    inline const auto& origin() const noexcept { return m_origin; }
    inline const auto& direction() const noexcept { return m_direction; }
    void setOrigin(const glm::vec3&) noexcept;
    void setDirection(const glm::vec3&) noexcept;
};

}

#endif /* Ray_h */
