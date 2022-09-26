//
//  ModelAnimation.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelAnimation_h
#define ModelAnimation_h

#include <ModelAnimation/Joint.h>
#include <ModelAnimation/JointAnimation.h>
#include <map>
#include <nlohmann/json.hpp>
#include <System/Math.h>

namespace Ice {

class ModelAnimation {

    float m_fLengthSeconds{ 0.0f };
    float m_fCurrentTime{ 0.0f };
    std::map<JointId, JointAnimation> m_mJointAnimations;
    
public:
    ModelAnimation() = default;
    
    inline auto lengthSeconds() const noexcept { return m_fLengthSeconds; }
    inline void setLengthSeconds(float fLength) noexcept { m_fLengthSeconds = fLength; }
    inline const auto& currentTime() const noexcept { return m_fCurrentTime; }
    inline auto& currentTime() noexcept { return m_fCurrentTime; }
    
    const auto& jointAnimations() const noexcept { return m_mJointAnimations; }
    auto& jointAnimations() noexcept { return m_mJointAnimations; }

    bool operator==(const ModelAnimation& rhs) const {
        return Math::equal(m_fLengthSeconds, rhs.m_fLengthSeconds) && Math::equal(m_fCurrentTime, rhs.m_fCurrentTime) &&  m_mJointAnimations == rhs.m_mJointAnimations;
    }

    friend void to_json(nlohmann::json& j, const Ice::ModelAnimation& ani) {
        j.emplace("lengthSeconds", ani.m_fLengthSeconds);
        j.emplace("jointAnimations", ani.m_mJointAnimations);
    }

    friend void from_json(const nlohmann::json& j, Ice::ModelAnimation& ani) {
        j.at("lengthSeconds").template get_to<float>(ani.m_fLengthSeconds);
        j.at("jointAnimations").template get_to<decltype(ModelAnimation::m_mJointAnimations)>(ani.m_mJointAnimations);
    }
};

}

#endif /* ModelAnimation_h */
