//
//  GravityAnimation.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 27.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Animation/GravityAnimation.h>
//#include "Controller.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Ice {

GravityAnimation::GravityAnimation(glm::vec3* pTarget, const glm::vec3& from, const glm::vec3& to) {
    m_nDuration = glm::length(from - to) * 100.0f;
    m_pTarget = pTarget;
    m_fromPosition = from;
    m_toPosition = to;
    m_direction = glm::normalize(to - from);
    m_fDelta = glm::length(from - to) / (float)m_nDuration;
}


void GravityAnimation::animate(int nCurrentTime) {
    //std::lock_guard guard(Ice::Controller::instance().camera().m_posMutex);
    auto newPos = m_fromPosition + (m_direction * (m_fDelta * (float)nCurrentTime));
    m_pTarget->x = newPos.x;
    m_pTarget->y = newPos.y;
    m_pTarget->z = newPos.z;
    if (nCurrentTime > m_nDuration) {
        m_bDone = true;
      //  Ice::Controller::instance().m_bHasGravityAnimation = false;
    }
}

bool GravityAnimation::done() {
    return m_bDone;
}

}
