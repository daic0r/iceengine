#include <Animation/JumpAnimation.h>
#include <mutex>
//#include "Controller.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace Ice {

/*!
* \brief
* [JumpAnimation::JumpAnimation]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
JumpAnimation::JumpAnimation(glm::vec3* pPos) {
	m_nDuration = 500;
	m_position = *pPos;
	m_pTarget = pPos;
}

/*!
* \brief
* [JumpAnimation::animate]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
void JumpAnimation::animate(int nCurrentTime) {
    //std::lock_guard guard(Ice::Controller::instance().camera().m_posMutex);
	m_pTarget->y = m_position.y + sin((M_PI / (float)m_nDuration) * (float)nCurrentTime);
	if (nCurrentTime > m_nDuration)
		m_bDone = true;
}

bool JumpAnimation::done() {
	return m_bDone;
}

}
