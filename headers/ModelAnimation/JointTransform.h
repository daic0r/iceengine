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
#include <nlohmann/json.hpp>
#include <Utils/serializables.h>

namespace Ice {

class JointTransform {
    glm::vec3 m_position;
    glm::quat m_rotation;
    glm::vec3 m_scaling;
    
public:
    
    JointTransform() = default;
    JointTransform(const glm::mat4&) noexcept;
    JointTransform(const glm::vec3&, const glm::quat&, const glm::vec3& s = glm::vec3{1.0f,1.0f,1.0f}) noexcept;
    JointTransform interpolate(const JointTransform& other, float fProgress) const noexcept;
    glm::mat4 toMatrix() const noexcept;

    bool operator==(const JointTransform& rhs) const {
        return m_position == rhs.m_position && m_rotation == rhs.m_rotation && m_scaling == rhs.m_scaling;
    }

    friend void to_json(nlohmann::json& j, const JointTransform& transform);
    friend void from_json(const nlohmann::json& j, JointTransform& transform);
};

inline void to_json(nlohmann::json& j, const JointTransform& transform) {
    j.emplace("position", transform.m_position);
    j.emplace("rotation", transform.m_rotation);
}

inline void from_json(const nlohmann::json& j, JointTransform& transform) {
    j.at("position").template get_to<glm::vec3>(transform.m_position);
    j.at("rotation").template get_to<glm::quat>(transform.m_rotation);
}

}

#endif /* JointTransform_h */
