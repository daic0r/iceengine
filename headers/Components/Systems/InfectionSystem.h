#pragma once

#include <Entities/EntityComponentSystem.h>
#include <Components/InfectionComponent.h>

namespace Ice {

class ParticleSystem;

class InfectionSystem : public EntityComponentSystem<InfectionComponent> {

	std::map<Entity, ParticleSystem*> m_vParticleSystems;

public:
	void onEntityAdded(Entity) noexcept override;
	void willRemoveComponent(Entity e, EntityComponentId id) noexcept override;
	bool update(float) noexcept;
};

}