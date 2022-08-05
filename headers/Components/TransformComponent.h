//
//  TransformComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TransformComponent_h
#define TransformComponent_h

#include <glm/mat4x4.hpp>
#include <Components/ComponentIds.h>
#include <iostream>
#include <Utils/serializables.h>

namespace Ice {

struct TransformComponent {
    static constexpr EntityComponentId id() noexcept { return TRANSFORM_COMPONENT; }
    
    glm::mat4 m_transform{ 1.0f };

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(transform, m_transform)
    END_DECLARE_SERIALIZABLE_ATTRIBS()

    friend std::ostream& operator<<(std::ostream& os, const TransformComponent& c);
    friend std::istream& operator>>(std::istream& is, TransformComponent& c);
};

inline std::ostream& operator<<(std::ostream& os, const TransformComponent& c) {
    os << TransformComponent::id() << " ";
    for (glm::length_t i = 0; i < 4; ++i) {
        for (glm::length_t j = 0; j < 4; ++j) {
            os << c.m_transform[i][j];
            if (i * j != 9)
                os << " ";
        }
    }
    return os;
}

inline std::istream& operator>>(std::istream& is, TransformComponent& c) {
    for (glm::length_t i = 0; i < 4; ++i) {
        for (glm::length_t j = 0; j < 4; ++j) {
            is >> c.m_transform[i][j];
        }
    }
    return is;
}

}

#endif /* TransformComponent_h */
