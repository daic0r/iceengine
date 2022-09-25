//
//  SkeletonComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SkeletonComponent_h
#define SkeletonComponent_h

#include <Components/ComponentIds.h>
#include <ModelAnimation/Joint.h>
#include <Utils/serializables.h>

namespace Ice {

struct SkeletonComponent {
    static constexpr EntityComponentId id() noexcept { return SKELETON_COMPONENT; }
    
    glm::mat4 m_invBindShapeMatrix;
    Joint m_rootJoint;

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(invBindShapeMatrix, m_invBindShapeMatrix)
    SERIALIZABLE_ATTRIB(rootJoint, m_rootJoint)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

}

#endif /* SkeletonComponent_h */
