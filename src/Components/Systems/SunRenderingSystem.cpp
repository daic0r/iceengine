#include <Components/Systems/SunRenderingSystem.h>
#include <Entities/EntityManager.h>
#include <Interfaces/ISunRenderer.h>

namespace Ice {

SunRenderingSystem::SunRenderingSystem() {
	m_pRenderer = systemServices.getSunRenderer();
}

void SunRenderingSystem::render(const RenderEnvironment& env) const noexcept {
	m_pRenderer->render(env);
}

}