#pragma once

#include <IceEngine.h>
#include <Entities/EntityComponentSystem.h>
#include "Components/BuildingComponent.h"
#include <glm/mat4x4.hpp>

namespace Ice {

class World;
struct TransformComponent;

class ICEENGINE_API BuildingSystem : public Ice::EntityComponentSystem<BuildingComponent>
{
	Ice::World* m_pWorld{ nullptr };
    Ice::Entity m_camEnt;
public:
    BuildingSystem(Ice::Entity camEntity) : m_camEnt{ camEntity } {}
	Ice::Entity createInstance(Ice::Entity blueprintEnt, const glm::mat4& = glm::mat4{ 1.0f });
	void setWorld(Ice::World* w) noexcept { m_pWorld = w; }
	void localizeBuildings() noexcept;
	void localizeBuilding(Ice::Entity) noexcept;
    void onEntityAdded(Ice::Entity) noexcept override;
};

}
