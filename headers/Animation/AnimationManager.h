#pragma once

#include <mutex>
#include <vector>
#include <thread>
#include <atomic>
#include <SDL2/SDL.h>
#include <iostream>

namespace Ice {

class Animation;

#define animationManager AnimationManager::instance()

class AnimationManager
{
	AnimationManager();
	~AnimationManager();
	struct AnimationInfo {
		int startTime = -1;
		std::unique_ptr<Animation> pAnimation;
        AnimationInfo() = default;
        AnimationInfo(AnimationInfo&&) = default;
        AnimationInfo& operator=(AnimationInfo&&) = default;
        ~AnimationInfo() { std::cout << "Dtor" << std::endl; }
	};

	void startThread();

public:
	static AnimationManager& instance();
	void addAnimation(Animation*);
	void shutDown();

private:
	std::atomic_bool m_bDone{ false };
	std::thread* m_pThread{ nullptr };
	std::recursive_mutex m_access;
	std::vector<AnimationInfo> m_vAnimations;

};

}
