#include <Animation/AnimationManager.h>
#include <Animation/Animation.h>
#include <iostream>

namespace Ice {

AnimationManager::AnimationManager() {
	startThread();
}

/*!
* \brief
* [AnimationManager::~AnimationManager]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
AnimationManager::~AnimationManager()
{
}

/*!
* \brief
* [AnimationManager::startThread]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
void AnimationManager::startThread()
{
	m_pThread = new std::thread([this] {
		while (!m_bDone) {
			m_access.lock();
			int nCurrentTime = SDL_GetTicks();
			std::vector<decltype(m_vAnimations)::iterator> toDelete;
            if (m_vAnimations.size() > 1) {
                std::cout << "I have " << m_vAnimations.size() << " animations" << std::endl;
            }
			for (auto iter = m_vAnimations.begin(); iter != m_vAnimations.end(); ++iter) {
				if (iter->pAnimation->done()) {
					toDelete.emplace_back(iter);
					continue;
				}
				iter->pAnimation->animate(nCurrentTime - iter->startTime);
			}
			m_access.unlock();
			SDL_Delay(5);
			m_access.lock();
			for (const auto& iter : toDelete) {
				m_vAnimations.erase(iter);
			}
			m_access.unlock();
		}
		std::cout << "Animation manager stopping..." << std::endl;
	});

}

/*!
* \brief
* [AnimationManager::instance]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
AnimationManager& AnimationManager::instance()
{
	static AnimationManager _inst;
	return _inst;
}

/*!
* \brief
* [AnimationManager::addAnimation]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
void AnimationManager::addAnimation(Animation* pAni)
{
	std::lock_guard guard(m_access);
	m_vAnimations.emplace_back(AnimationInfo{});
	auto& info = m_vAnimations.back();
	info.startTime = SDL_GetTicks();
	info.pAnimation = std::move(std::unique_ptr<Animation>{ pAni });
}

/*!
* \brief
* [AnimationManager::shutDown]
*
* \author matthias.gruen
* \date 2020/08/27
* [8/27/2020 matthias.gruen]
*/
void AnimationManager::shutDown()
{
	m_bDone = true;
	if (m_pThread != nullptr) {
		m_pThread->join();
		delete m_pThread;
	}
}

}
