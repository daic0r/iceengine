//
//  JointTransform.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef JointTransform_h
#define JointTransform_h

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>

namespace Ice {

class JointTransform {
    glm::vec3 m_position;
    glm::quat m_rotation;
    
public:
    
    JointTransform() = default;
    JointTransform(const glm::mat4&) noexcept;
    JointTransform(const glm::vec3&, const glm::quat&) noexcept;
    JointTransform interpolate(const JointTransform& other, float fProgress) const noexcept;
    glm::mat4 toMatrix() const noexcept;
};

}

#endif /* JointTransform_h */
