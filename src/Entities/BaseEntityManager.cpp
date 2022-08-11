#include <Entities/BaseEntityManager.h>
#include <algorithm>
#include <Entities/BaseComponentSystem.h>
#include <Interfaces/IEntityComponentBuffer.h>
#include <iostream>
#include <array>
#include <Interfaces/IStreamSaver.h>
#include <System/ThreadPool.h>
#include <chrono>
#include <mutex>

namespace Ice {

Entity BaseEntityManager::createEntity() noexcept {
    std::scoped_lock<std::recursive_mutex> lck{ m_cmpIdMtx };
	m_mComponentIds[currentScene()][m_nNextId] = std::set<EntityComponentId>{};
	return m_nNextId++;
}

bool BaseEntityManager::deleteEntity(Entity e) noexcept {
    std::scoped_lock<std::recursive_mutex> lck{ m_cmpIdMtx };

    auto compIter = m_mComponentIds.find(currentScene());

	auto entIter = compIter->second.find(e);
	if (entIter == compIter->second.end())
		return false;

    const auto notifyAll = [e, this](BaseComponentSystem* pSys) {
        if (pSys->entities(currentScene()).contains(e)) {
            for (auto id : pSys->m_sComponentIds)
                pSys->willRemoveComponent(e, id);
        }
    };
	for (auto pSystem : m_vEntityComponentSystems) {
        notifyAll(pSystem);
		pSystem->removeEntity(e, currentScene());
	}
    for (auto pSystem : m_vNonActiveSystems) {
        notifyAll(pSystem);
        pSystem->removeEntity(e, currentScene());
    }
    for (auto pListener : m_vEntityComponentBuffers)
		pListener->onRemoveEntity(e, currentScene());

	compIter->second.erase(entIter);
	return true;
}

/*!
* \brief
* [EntityManager::registerComponentBuffer]
*
* \author Matthias.Gruen
* \date 2020/10/01
* [10/1/2020 Matthias.Gruen]
*/
void BaseEntityManager::registerComponentBuffer(IEntityComponentBuffer* pListener) noexcept {
	m_vEntityComponentBuffers.emplace_back(pListener);
}

void BaseEntityManager::notifyComponentAdded(Entity ent) const noexcept {
    const auto& entityComponents = m_mComponentIds.at(currentScene()).at(ent);
    
    std::array<const std::vector<Ice::BaseComponentSystem*>*, 2> arLists = { &m_vEntityComponentSystems, &m_vNonActiveSystems };

    for (int i = 0; i < 2; ++i) {
        for (auto pListener : *arLists[i]) {
            if (std::includes(entityComponents.begin(), entityComponents.end(), pListener->componentIds().begin(), pListener->componentIds().end())) {
                //std::cout << "System must handle this entity now: " << ent << std::endl;
                pListener->addEntity(ent, currentScene());
            }
        }
    }
}

/*!
* \brief
* [EntityManager::notifyComponentRemoved]
*
* \author Matthias.Gruen
* \date 2020/10/01
* [10/1/2020 Matthias.Gruen]
*/
void BaseEntityManager::notifyComponentRemoved(Entity ent) const noexcept {
    const auto& entityComponents = m_mComponentIds.at(currentScene()).at(ent);

    std::array<const std::vector<Ice::BaseComponentSystem*>*, 2> arLists = { &m_vEntityComponentSystems, &m_vNonActiveSystems };

    for (int i = 0; i < 2; ++i) {
        for (auto pListener : *arLists[i]) {
            if (!std::includes(entityComponents.begin(), entityComponents.end(), pListener->componentIds().begin(), pListener->componentIds().end())) {
                //std::cout << "System is not concerned with this entity anymore: " << ent << std::endl;
                pListener->removeEntity(ent, currentScene());
            }
        }
    }
}

void BaseEntityManager::notifyWillRemoveComponent(Entity ent, EntityComponentId compId) const noexcept {
    const auto& entityComponents = m_mComponentIds.at(currentScene()).at(ent);

    std::array<const std::vector<Ice::BaseComponentSystem*>*, 2> arLists = { &m_vEntityComponentSystems, &m_vNonActiveSystems };

    for (int i = 0; i < 2; ++i) {
        for (auto pListener : *arLists[i]) {
            if (std::includes(entityComponents.begin(), entityComponents.end(), pListener->componentIds().begin(), pListener->componentIds().end()) &&
                std::find(pListener->componentIds().begin(), pListener->componentIds().end(), compId) != pListener->componentIds().end()) {
                //std::cout << "System is not concerned with this entity anymore: " << ent << std::endl;
                pListener->willRemoveComponent(ent, compId);
            }
        }
    }
}

void BaseEntityManager::notifySystemsInitialized() const noexcept {
    std::array<const std::vector<Ice::BaseComponentSystem*>*, 2> arLists = { &m_vEntityComponentSystems, &m_vNonActiveSystems };

    for (int i = 0; i < 2; ++i) {
        for (auto pListener : *arLists[i]) {
			pListener->onSystemsInitialized();
        }
    }
}

bool BaseEntityManager::update(float fDeltaTime) {
    static std::once_flag flag;
    //static std::chrono::milliseconds dur;
    //static float fTimeElapsed{ 0.0f };
    //static size_t nFps{ 0 };
    bool bRet{ true };

    std::call_once(flag, [this]() { m_vFutures.reserve(m_vEntityComponentSystems.size()); });

	m_vFutures.clear();
    //using namespace std::chrono;
    //auto be = high_resolution_clock::now();
	size_t nLastUnfinishedIndex = 0;
	int nLastOrder = 0;
	while (nLastUnfinishedIndex < m_vEntityComponentSystems.size()) {
		for (size_t i = nLastUnfinishedIndex; i < m_vEntityComponentSystems.size(); ++i) {
			auto pSystem = m_vEntityComponentSystems[i];
			if (pSystem->executionOrder() != nLastOrder) {
				nLastOrder = pSystem->executionOrder();
				break;
			}
			m_vFutures.push_back(threadPool.queue([fDeltaTime, pSystem]() {
				return pSystem->update(fDeltaTime);
				}));
			nLastOrder = pSystem->executionOrder();
			++nLastUnfinishedIndex;
		}
		for (auto& fut : m_vFutures)
			bRet &= fut.get();
		m_vFutures.clear();
	}
    //vFutures.clear();
    //auto en = duration_cast<milliseconds>(high_resolution_clock::now() - be);
    //dur += en;
    //fTimeElapsed += fDeltaTime;
    //++nFps;
    //if (fTimeElapsed > 1.0f) {
    //    std::cout << "Took " << static_cast<float>(dur.count()) / static_cast<float>(nFps) << "ms" << std::endl;
    //    fTimeElapsed = 0.0f;
    //    dur = milliseconds{ 0 };
    //    nFps = 0;
    //}
    return bRet;
}

void BaseEntityManager::registerSaver(std::variant<IStreamSaver*, IJSONSaver*> pSaveable) noexcept {
    m_vSavers.push_back(pSaveable);
}

}