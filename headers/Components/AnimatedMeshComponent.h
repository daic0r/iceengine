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
#include <Utils/serializables.h>

namespace Ice {

struct AnimatedMeshComponent {
    static constexpr EntityComponentId id() noexcept { return ANIMATED_MESH_COMPONENT; }
    
    std::vector<glm::ivec4> m_vJointIds;
    std::vector<glm::vec4> m_vWeights;
    std::uint16_t m_nNumJoints{ 0 };

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(jointIds, m_vJointIds)
    SERIALIZABLE_ATTRIB(weights, m_vWeights)
    SERIALIZABLE_ATTRIB(numJoints, m_nNumJoints)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

}

#endif /* AnimatedMeshComponent_h */
