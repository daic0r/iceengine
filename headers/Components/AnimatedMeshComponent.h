//
//  AnimatedMeshComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedMeshComponent_h
#define AnimatedMeshComponent_h

#include <Components/ComponentIds.h>
#include <vector>
#include <glm/vec4.hpp>
#include <cstdint>

namespace Ice {

struct AnimatedMeshComponent {
    static constexpr EntityComponentId id() noexcept { return ANIMATED_MESH_COMPONENT; }
    
    std::vector<glm::ivec4> m_vJointIds;
    std::vector<glm::vec4> m_vWeights;
    std::uint16_t m_nNumJoints{ 0 };
};

}

#endif /* AnimatedMeshComponent_h */
