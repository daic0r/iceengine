//
//  JointTransform.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ModelAnimation/JointTransform.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Ice {

JointTransform::JointTransform(const glm::mat4& transform) noexcept {
    m_position = glm::vec3{ transform[3][0], transform[3][1], transform[3][2] };
    m_rotation = glm::toQuat(transform);
}

JointTransform::JointTransform(const glm::vec3& p, const glm::quat& r) noexcept
: m_position{ p },
m_rotation{ r }
{
}

JointTransform JointTransform::interpolate(const JointTransform &other, float fProgress) const noexcept {
    JointTransform ret{ m_position + (other.m_position - m_position) * fProgress, glm::slerp(m_rotation, other.m_rotation, fProgress) };
    
    return ret;
}

glm::mat4 JointTransform::toMatrix() const noexcept {
    glm::mat4 ret;
    auto rotMat = glm::toMat4(m_rotation);;
    ret = glm::translate(glm::mat4{ 1.0f }, m_position) * rotMat;
    return ret;
}

}
