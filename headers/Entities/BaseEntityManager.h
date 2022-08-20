#pragma once

#include <IceEngine.h>
#include <Entities/Entity.h>
#include <unordered_map>
#include <vector>
#include <set>
#include <mutex>
#include <future>
#include <Entities/BaseComponentSystem.h>
#include <variant>
#include <algorithm>
#include <System/ThreadPool.h>

namespace Ice {

class IEntityComponentBuffer;
class IStreamSaver;
class IJSONSaver;

class ICEENGINE_API BaseEntityManager {
protected:
	static constexpr Entity MAX_ENTITIES{ 1024 * 1024 };
	static constexpr Entity INVALID_ID{ 0 };

	SceneId m_nCurrentScene{ 0 };

	Entity m_nNextId{ 1 };

    mutable std::recursive_mutex m_cmpIdMtx;
	std::unordered_map<SceneId, std::unordered_map<Entity, std::set<EntityComponentId>>> m_mComponentIds;
	
    std::vector<std::future<bool>> m_vFutures;

    std::vector<BaseComponentSystem*> m_vEntityComponentSystems;
    std::vector<BaseComponentSystem*> m_vNonActiveSystems;
	std::vector<IEntityComponentBuffer*> m_vEntityComponentBuffers;
    std::vector<std::variant<IStreamSaver*, IJSONSaver*>> m_vSavers;
    ThreadPool m_threadPool{};
    
public:
	inline SceneId currentScene() const noexcept { return m_nCurrentScene; }
	inline void setCurrentScene(SceneId id) noexcept { m_nCurrentScene = id; }

	void registerComponentBuffer(IEntityComponentBuffer*) noexcept;
    void registerSaver(std::variant<IStreamSaver*, IJSONSaver*>) noexcept;
    inline auto savers() const noexcept -> const decltype(m_vSavers)& { return m_vSavers; }
    
	void notifyComponentAdded(Entity) const noexcept;
	void notifyComponentRemoved(Entity) const noexcept;
    void notifyWillRemoveComponent(Entity, EntityComponentId) const noexcept;
	void notifySystemsInitialized() const noexcept;

	Entity createEntity() noexcept;
	bool deleteEntity(Entity) noexcept;
    
    bool update(float fDeltaTime);
    
    inline const auto& componentSystems() const noexcept { return m_vEntityComponentSystems; }

    template<bool IsActive> void registerComponentSystem(BaseComponentSystem*) noexcept;
    template<bool IsActive> void registerUserComponentSystem(BaseComponentSystem*) noexcept;
    template<typename ComponentSystem, bool IsActive> ComponentSystem* getSystem() const noexcept;
};

template<typename ComponentSystem, bool IsActive> ComponentSystem* BaseEntityManager::getSystem() const noexcept {
    const auto& list = [this]() constexpr {
        if constexpr(IsActive == true)
            return m_vEntityComponentSystems;
        else
            return m_vNonActiveSystems;
    }();
    
    for (auto pSystem : list) {
        ComponentSystem* pRet = dynamic_cast<ComponentSystem*>(pSystem);
        if (pRet != nullptr)
            return pRet;
    }
    return nullptr;
}

template<bool IsActive> void BaseEntityManager::registerComponentSystem(BaseComponentSystem* pListener) noexcept {
	if constexpr (IsActive == true) {
		m_vEntityComponentSystems.emplace_back(pListener);
		std::sort(m_vEntityComponentSystems.begin(), m_vEntityComponentSystems.end(), [](BaseComponentSystem* a, BaseComponentSystem* b) {
			return a->executionOrder() < b->executionOrder();
		});
	}
    else
        m_vNonActiveSystems.emplace_back(pListener);
}

template<bool IsActive> void BaseEntityManager::registerUserComponentSystem(BaseComponentSystem* pListener) noexcept {
    if constexpr(IsActive == true)
        m_vEntityComponentSystems.emplace_back(pListener);
    else
        m_vNonActiveSystems.emplace_back(pListener);
}

}
