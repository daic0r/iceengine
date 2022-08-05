//
//  MoveForwardAnimation.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 27.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Animation/MoveAnimation.h>
//#include "../System/Controller.h"
#include <mutex>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>

namespace Ice {

MoveAnimation::MoveAnimation(glm::vec3* pPos, const glm::vec3& dir, float fAmount) {
    m_nDuration = 200;
    m_position = *pPos;
    m_direction = dir;
    m_pTarget = pPos;
    m_fAmount = fAmount;
    m_fDelta = m_fAmount / (float)m_nDuration;
}

void MoveAnimation::animate(int nCurrentTime) {
    //std::lock_guard guard(Ice::Controller::instance().camera().m_posMutex);
    auto newPos = *m_pTarget + (m_direction * (m_fDelta * (float)nCurrentTime));
    m_pTarget->x = newPos.x;
    m_pTarget->y = newPos.y;
    m_pTarget->z = newPos.z;
    if (nCurrentTime > m_nDuration)
        m_bDone = true;
}

bool MoveAnimation::done() {
    return m_bDone;
}

}
