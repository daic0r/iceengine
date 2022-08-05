#include <Components/Systems/InfectionSystem.h>
#include <Components/TransformComponent.h>
#include <Components/ParticleSystemComponent.h>
#include <Entities/EntityManager.h>
#include <glm/vec3.hpp>
#include <Particles/ParticleSystem.h>
#include <System/SystemServices.h>
#include <Interfaces/ITextureManager.h>
#include <System/Config.h>
#include <System/BaseTexture.h>

namespace Ice {

void InfectionSystem::onEntityAdded(Entity e) noexcept {
	const auto& transComp = entityManager.getComponent<TransformComponent>(e);
	glm::vec3 pos{ transComp.m_transform[3][0], transComp.m_transform[3][1], transComp.m_transform[3][2] };

	//(const glm::vec3& pos, int nPps, float fGravity, float fLifeLength, float fSpeed, BaseTexture* pTexture)

	auto pTexMan = systemServices.getTextureManager();
	auto pTex = pTexMan->registerTexture(systemServices.getConfig()->prependAssetPath("Particles/particleAtlas.png"));
	pTex->setRows(4);
	pTex->setCols(4);

	ParticleSystemComponent psComp{ ParticleSystem{ pos, 50, .1f, 5.0f, 10.0f, pTex } };
	entityManager.addComponent(e, psComp);
}

void InfectionSystem::willRemoveComponent(Entity e, EntityComponentId id) noexcept {
	if (id == INFECTION_COMPONENT) {
		entityManager.removeComponent<ParticleSystemComponent>(e);
	}
}

bool InfectionSystem::update(float fDeltaTime) noexcept
{
	return true;
}

}