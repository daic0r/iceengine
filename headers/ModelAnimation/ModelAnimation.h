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

namespace Ice {

class ModelAnimation {

    float m_fLengthSeconds{ 0.0f };
    float m_fCurrentTime{ 0.0f };
    std::map<JointId, JointAnimation> m_mJointAnimations;
    
public:
    
    inline auto lengthSeconds() const noexcept { return m_fLengthSeconds; }
    inline void setLengthSeconds(float fLength) noexcept { m_fLengthSeconds = fLength; }
    inline const auto& currentTime() const noexcept { return m_fCurrentTime; }
    inline auto& currentTime() noexcept { return m_fCurrentTime; }
    
    const auto& jointAnimations() const noexcept { return m_mJointAnimations; }
    auto& jointAnimations() noexcept { return m_mJointAnimations; }
};

}

#endif /* ModelAnimation_h */
