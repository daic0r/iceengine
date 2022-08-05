//
//  ProjectionComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ProjectionComponent_h
#define ProjectionComponent_h

#include <Components/ComponentIds.h>
#include <glm/mat4x4.hpp>

namespace Ice {

struct ProjectionComponent {
    static constexpr EntityComponentId id() noexcept { return PROJECTION_COMPONENT; }
    
    glm::mat4 m_projection;
};

}

#endif /* ProjectionComponent_h */
