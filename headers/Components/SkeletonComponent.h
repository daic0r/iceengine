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

namespace Ice {

struct SkeletonComponent {
    static constexpr EntityComponentId id() noexcept { return SKELETON_COMPONENT; }
    
    glm::mat4 m_invBindShapeMatrix;
    Joint m_rootJoint;
};

}

#endif /* SkeletonComponent_h */
