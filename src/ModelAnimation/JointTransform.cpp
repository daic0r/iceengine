//
//  JointTransform.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ModelAnimation/JointTransform.h>
#include <glm/gtc/matrix_transform.hpp>
#include <System/Math.h>

namespace Ice {

JointTransform::JointTransform(const glm::mat4& transform) noexcept {
    m_scaling = Math::getScaleFactorsFromMatrix(transform);
    const auto mat = Math::descale(transform, m_scaling);
    m_position = glm::vec3{ mat[3][0], mat[3][1], mat[3][2] };
    m_rotation = glm::toQuat(mat);
}

JointTransform::JointTransform(const glm::vec3& p, const glm::quat& r, const glm::vec3& s) noexcept
: m_position{ p },
m_rotation{ r },
m_scaling{ s }
{
}

JointTransform JointTransform::interpolate(const JointTransform &other, float fProgress) const noexcept {
    JointTransform ret{ m_position + (other.m_position - m_position) * fProgress, glm::slerp(m_rotation, other.m_rotation, fProgress), m_scaling + (other.m_scaling - m_scaling) * fProgress };
    
    return ret;
}

glm::mat4 JointTransform::toMatrix() const noexcept {
    glm::mat4 ret;
    auto scaleMat = glm::scale(glm::mat4{1.0f}, m_scaling);
    auto rotMat = glm::toMat4(m_rotation);
    ret = glm::translate(glm::mat4{ 1.0f }, m_position) * rotMat * scaleMat;
    return ret;
}

}
