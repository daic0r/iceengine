//
//  Joint.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Joint_h
#define Joint_h

#include <glm/mat4x4.hpp>
#include <string>
#include <cstdint>
#include <vector>
#include <nlohmann/json.hpp>

namespace Ice {

using JointId = std::uint16_t;

class Joint  {

    JointId m_nId;
    std::string m_strName;
    glm::mat4 m_invBindTransform;   // original joint model space representation to local joint space
    std::vector<Joint> m_vChildren;
    
    glm::mat4 m_bindTransform; // original pose
    glm::mat4 m_transform; // current transform in local space

public:
    Joint() = default;
    Joint(std::uint16_t nId, const std::string& strName, const glm::mat4& invBindTransform) noexcept : m_nId{ nId }, m_strName{ strName }, m_invBindTransform{ invBindTransform } {}
     
    auto id() const noexcept { return m_nId; }
    const auto& name() const noexcept { return m_strName; }
    const auto& invBindTransform() const noexcept { return m_invBindTransform; }
    const auto& bindTransform() const noexcept { return m_bindTransform; }
    const auto& transform() const noexcept { return m_transform; }
    const auto& children() const noexcept { return m_vChildren; }
    auto& children() noexcept { return m_vChildren; }

    void setId(std::uint16_t nId) noexcept { m_nId = nId; }
    void setName(std::string_view strName) { m_strName = strName; }
    void setInvBindTransform(const glm::mat4& b) noexcept { m_invBindTransform = b; }
    inline void setBindTransform(const glm::mat4& b) noexcept { m_bindTransform = b; }
    inline void setTransform(const glm::mat4& t) noexcept { m_transform = t; }
    inline void addChild(const Joint& j) noexcept { m_vChildren.emplace_back(j); }

    bool operator==(const Joint& rhs) const {
        return m_nId == rhs.m_nId && m_strName == rhs.m_strName && m_invBindTransform == rhs.m_invBindTransform && m_vChildren == rhs.m_vChildren && m_bindTransform == rhs.m_bindTransform;
    }

    friend void to_json(nlohmann::json& j, const Ice::Joint& v);
    friend void from_json(const nlohmann::json& j, Ice::Joint& v);
};

inline void to_json(nlohmann::json& j, const Ice::Joint& v) {
    j.emplace("id", v.m_nId);
    j.emplace("name", v.m_strName);
    j.emplace("invBindTransform", v.m_invBindTransform);
    j.emplace("bindTransform", v.m_bindTransform);
    j.emplace("children", v.m_vChildren);
}

inline void from_json(const nlohmann::json& j, Ice::Joint& v) {
    v.m_nId = j.at("id").template get<std::uint16_t>();
    v.m_strName = j.at("name").template get<std::string>();
    v.m_invBindTransform = j.at("invBindTransform").template get<glm::mat4>();
    v.m_bindTransform = j.at("bindTransform").template get<glm::mat4>();
    v.children() = j.at("children").template get<std::vector<Ice::Joint>>(); 
}

}

#endif /* Joint_h */
