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

    bool operator==(const JointAnimation& rhs) const {
        return m_mJointTransforms == rhs.m_mJointTransforms;
    }

    friend void to_json(nlohmann::json& j, const JointAnimation&);
    friend void from_json(const nlohmann::json& j, JointAnimation&);
};

inline void to_json(nlohmann::json& j, const JointAnimation& animation) {
    j.emplace("jointTransforms", animation.m_mJointTransforms);
}

inline void from_json(const nlohmann::json& j, JointAnimation& animation) {
    j.at("jointTransforms").template get_to<std::map<float, JointTransform>>(animation.m_mJointTransforms);
}

}

#endif /* JointAnimation_h */
