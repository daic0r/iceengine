#pragma once

#include <Entities/EntityComponentSystem.h>
#include <Components/SunComponent.h>

namespace Ice {

struct RenderEnvironment;

class SunRenderingSystem : public EntityComponentSystem<SunComponent> {

	ISunRenderer* m_pRenderer{ nullptr };

public:
	SunRenderingSystem();
	void render(const RenderEnvironment&) const noexcept;

};

}