#include <Components/Systems/ParticleSystemSystem.h>
#include <Components/TransformComponent.h>
#include <Interfaces/IParticleRenderer.h>
#include <System/SystemServices.h>
#include <Entities/EntityManager.h>

namespace Ice {

ParticleSystemSystem::ParticleSystemSystem() {
	m_pRenderer = systemServices.getParticleRenderer();
}

bool ParticleSystemSystem::update(float fDeltaTime) noexcept {
	for (auto e : entities(entityManager.currentScene())) {
		auto& sysComp = entityManager.getComponent<ParticleSystemComponent>(e);
		const auto& transformComp = entityManager.getComponent<TransformComponent>(e);
		auto pos = glm::vec3{ transformComp.m_transform[3][0], transformComp.m_transform[3][1], transformComp.m_transform[3][2] } + sysComp.m_positionOffset;
		sysComp.m_particleSystem.setPosition(pos);

		sysComp.m_particleSystem.update(fDeltaTime);
	}
	return true;
}

void ParticleSystemSystem::onEntityAdded(Entity e) noexcept {
	auto& sysComp = entityManager.getComponent<ParticleSystemComponent>(e);
	m_vRenderBuffer.push_back(&sysComp.m_particleSystem);
}

void ParticleSystemSystem::willRemoveComponent(Entity e, EntityComponentId id) noexcept {
	if (id == PARTICLE_SYSTEM_COMPONENT) {
		auto& sysComp = entityManager.getComponent<ParticleSystemComponent>(e);
		std::erase(m_vRenderBuffer, &sysComp.m_particleSystem);
	}
}

void ParticleSystemSystem::render(const RenderEnvironment& env) noexcept {
	m_pRenderer->render(env, m_vRenderBuffer);
}

}