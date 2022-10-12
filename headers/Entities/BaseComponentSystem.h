#pragma once

#include <IceEngine.h>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <mutex>
#include <Entities/Entity.h>

namespace Ice {

class BaseComponentSystem
{
	friend class BaseEntityManager;

	std::recursive_mutex m_entityMtx;
	std::unordered_map<SceneId, std::set<Entity>> m_sEntities;
	
	std::mutex m_cmpIdMtx;
	std::set<EntityComponentId> m_sComponentIds;

public:
	void addEntity(Entity, SceneId) noexcept;
	void removeEntity(Entity, SceneId) noexcept;
	auto& componentIds() noexcept { return m_sComponentIds; }
	std::set<Entity>& entities(SceneId scId) noexcept { return m_sEntities[scId]; }
	const std::set<Entity>& entities(SceneId scId) const noexcept { return m_sEntities.at(scId); }
    virtual bool update(float fDeltaTime) { return true; }
    virtual void onEntityAdded(Entity) noexcept {}
	virtual void onSystemsInitialized() noexcept {}
    virtual void willRemoveComponent(Entity, EntityComponentId) noexcept {}
	virtual int executionOrder() const noexcept { return 10; }
};

}
