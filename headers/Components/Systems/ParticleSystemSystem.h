#pragma once

#include <Components/ParticleSystemComponent.h>
#include <Entities/EntityComponentSystem.h>
#include <Entities/EntityManager.h>

namespace Ice {

struct RenderEnvironment;
class IParticleRenderer;

class ParticleSystemSystem : public EntityComponentSystem<ParticleSystemComponent> {
	IParticleRenderer* m_pRenderer{ nullptr };
	std::vector<ParticleSystem*> m_vRenderBuffer;

public:
	ParticleSystemSystem();

	void render(const RenderEnvironment&) noexcept;

	bool update(float fDeltaTime) noexcept override;
	void onEntityAdded(Entity) noexcept override;
	void willRemoveComponent(Entity, EntityComponentId) noexcept override;
};

}