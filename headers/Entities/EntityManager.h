//
//  EntityManager.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef EntityManager_h
#define EntityManager_h

#include <IceEngine.h>
#include <Entities/Entity.h>
#include <Entities/BaseEntityManager.h>
#include <Components/SharedComponent.h>
#include <Components/TagComponent.h>
#include <Components/LinkedEntitiesComponent.h>
#include <assert.h>

namespace Ice {

class EntityManager : public BaseEntityManager {
    static bool m_bInstanced;

    template<typename Component>
    struct impl {
        static Component& getComponent(EntityManager&, Entity, const std::function<void(Component&)>& = nullptr);
        static void addComponent(EntityManager& man, Entity e, const Component& c) noexcept;
    };
    template<typename Component>
    struct impl<SharedComponent<Component>> {
        static SharedComponent<Component>& getComponent(EntityManager&, Entity, const std::function<void(SharedComponent<Component>&)>& = nullptr);
        static void addComponent(EntityManager& man, Entity e, const SharedComponent<Component>& c) noexcept;
    };
public:
    EntityManager() noexcept;
    
    // Operations on entities
    template<typename Component> void addComponent(Entity e, const Component& c) noexcept;
    template<typename Component> Component& getComponent(Entity, const std::function<void(Component&)>& = nullptr);
    template<typename Component> Component& getSharedComponentOr(Entity, const std::function<void(Component&)>& = nullptr);
    template<typename Component> bool hasComponent(Entity) const noexcept;
    template<typename Component> bool removeComponent(Entity) noexcept;
};

}

#include <Entities/EntityComponentBuffer.h>

namespace Ice {

template<typename Component> Component& EntityManager::impl<Component>::getComponent(EntityManager& man, Entity e, const std::function<void(Component&)>& func) {
    
    return EntityComponentBuffer<Component>::instance().getComponent(e, func, man.currentScene());
}

template<typename Component> SharedComponent<Component>& EntityManager::impl<SharedComponent<Component>>::getComponent(EntityManager& man, Entity e, const std::function<void(SharedComponent<Component>&)>& func) {
    
    auto& comp = EntityComponentBuffer<SharedComponent<Component>>::instance().getComponent(e, func, man.currentScene());
    if (comp.m_pComponent == nullptr) {
        auto refEnt = EntityComponentBuffer<TagComponent>::instance().findEntity([refId=comp.m_strReferenceEntityTag](const TagComponent& c) {
            return c.m_strTag == refId;
        });
		comp.m_refEnt = refEnt;
        comp.m_pComponent = &man.getComponent<Component>(refEnt);
    }
    return comp;
}

template<typename Component> Component& EntityManager::getSharedComponentOr(Entity e, const std::function<void(Component&)>& func) {

    if (hasComponent<Component>(e)) {
        return getComponent<Component>(e, func);
    }
    auto& sharedComp = getComponent<SharedComponent<Component>>(e);
	if (func)
		func(*sharedComp.m_pComponent);
    return *sharedComp.m_pComponent;
}

template<typename Component> void EntityManager::impl<Component>::addComponent(EntityManager& man, Entity e, const Component& c) noexcept {
    std::scoped_lock<std::recursive_mutex> lck{ man.m_cmpIdMtx };
    if (!EntityComponentBuffer<Component>::instance().addComponent(e, c, man.currentScene()))
        return;

    auto& entityComponents = man.m_mComponentIds[man.currentScene()][e];
    entityComponents.emplace(Component::id());
    man.notifyComponentAdded(e);
}

template<typename Component> void EntityManager::impl<SharedComponent<Component>>::addComponent(EntityManager& man, Entity e, const SharedComponent<Component>& c) noexcept {
    std::scoped_lock<std::recursive_mutex> lck{ man.m_cmpIdMtx };
    if (!EntityComponentBuffer<SharedComponent<Component>>::instance().addComponent(e, c, man.currentScene()))
        return;

    auto& entityComponents = man.m_mComponentIds[man.currentScene()][e];

    // Adding both IDs here, the normal one so that the entity will be added to the correct system
    // Do we need the Shared one too?
    entityComponents.emplace(Component::id());
    entityComponents.emplace(SharedComponent<Component>::id());
    man.notifyComponentAdded(e);

	///////////////////////////////////////////////////
	/// Append this entity to the reference entity's
	/// linked entity list
	auto refEnt = EntityComponentBuffer<TagComponent>::instance().findEntity([refId=c.m_strReferenceEntityTag](const TagComponent& c) {
        return c.m_strTag == refId;
    });
	if (refEnt != INVALID_ENTITY_ID) {
		// Update linked entities component in reference entity to contain this entity
		auto& linkedComp = std::invoke([refEnt, &man]() -> decltype(auto) {
			if (!EntityComponentBuffer<LinkedEntitiesComponent>::instance().hasComponent(refEnt, man.currentScene())) {
				EntityComponentBuffer<LinkedEntitiesComponent>::instance().addComponent(refEnt, LinkedEntitiesComponent{});
			}
			return EntityComponentBuffer<LinkedEntitiesComponent>::instance().getComponent(refEnt, nullptr, man.currentScene());
		});
		linkedComp.m_sLinkedEntities.insert(e);
	}
	///////////////////////////////////////////////////
}

template<typename Component> void EntityManager::addComponent(Entity e, const Component& c) noexcept {
    impl<Component>::addComponent(*this, e, c);
}

template<typename Component> Component& EntityManager::getComponent(Entity e, const std::function<void(Component&)>& func) {
    //return EntityComponentBuffer<Component>::instance().getComponent(e, func, currentScene());
    return impl<Component>::getComponent(*this, e, func);
}

template<typename Component> bool EntityManager::hasComponent(Entity e) const noexcept {
    return EntityComponentBuffer<Component>::instance().hasComponent(e, currentScene());
}

template<typename Component> bool EntityManager::removeComponent(Entity e) noexcept {
    std::scoped_lock<std::recursive_mutex> lck{ m_cmpIdMtx };

    auto sceneIter = m_mComponentIds.find(currentScene());
    
    if (sceneIter == m_mComponentIds.end())
        return false;

    auto entiter = sceneIter->second.find(e);
    if (entiter == sceneIter->second.end())
        return false;
    
    auto& entityComponents = entiter->second;
    auto compIter = entityComponents.find(Component::id());
    if (compIter == entityComponents.end())
        return false;
    notifyWillRemoveComponent(e, Component::id());
    if (!EntityComponentBuffer<Component>::instance().removeComponent(e, currentScene()))
        return false;

    entityComponents.erase(compIter);
    notifyComponentRemoved(e);
    
    if (entityComponents.size() == 0)
        sceneIter->second.erase(e);
    
    return true;
}

}

#endif /* EntityManager_h */
