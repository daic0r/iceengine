//
//  JointAnimation.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef JointAnimation_h
#define JointAnimation_h

#include <map>
#include <ModelAnimation/Joint.h>
#include <ModelAnimation/JointTransform.h>

namespace Ice {

class JointAnimation {

    std::map<float, JointTransform> m_mJointTransforms; // transforms at different points in time
    
public:
    
    inline auto& jointTransforms() noexcept { return m_mJointTransforms; }
    inline const auto& jointTransforms() const noexcept { return m_mJointTransforms; }
};

}

#endif /* JointAnimation_h */
