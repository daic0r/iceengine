//
//  EntityComponentBufferBase.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef EntityComponentBuffer_h
#define EntityComponentBuffer_h

#include <IceEngine.h>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <Entities/Entity.h>
#include <System/StreamTools.h>
#include <utility>
#include <mutex>
#include <Utils/TemplateHelpers.h>
#include <nlohmann/json.hpp>

#include <Interfaces/IEntityComponentBuffer.h>
#include <Interfaces/IStreamSaver.h>
#include <Interfaces/IJSONSaver.h>

namespace std {
	class type_info;
}

namespace Ice {

class EntityManager;

//////////////////////////////////////////////////////////////////////
// SaveableBase
//
// Base class which only is defined for the second type = void and a
// template alias as a helper. The constructor registers itself with the
// entity manager.
template<typename Component>
using SaveableEnablerType = typename std::enable_if_t<has_ostream_operator_v<Component>, void>;

template<typename Component, typename Aux = void> class SaveableBase {};

template<typename Component>
class SaveableBase<Component, typename std::enable_if_t<has_ostream_operator_v<Component> && !IsSavable<Component>, void>> : public IStreamSaver {
public:
    SaveableBase() noexcept;
};

template<typename Component>
class SaveableBase<Component, typename std::enable_if_t<IsSavable<Component>, void>> : public IJSONSaver {
public:
    SaveableBase() noexcept;
};
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// EntityComponentBufferBase
//
// Contains all the functionality of a component buffer. Not to be used
// directly. The derived classes EntityComponentBuffer are to be used.
template<typename Component> class EntityComponentBufferBase : public IEntityComponentBuffer {
protected:
    static constexpr int MAX_COMPONENTS = 0xFFFF;

    mutable std::mutex m_dataMtx;
    std::unordered_map<SceneId, std::vector<Component>> m_vCompData;
    std::unordered_map<SceneId, std::unordered_map<size_t, Entity>> m_mIndex2Entity;
    std::unordered_map<SceneId, std::unordered_map<Entity, size_t>> m_mEntity2Index;

    EntityComponentBufferBase() noexcept;
    
public:
    
    bool addComponent(Entity entId, const Component& comp, SceneId id = 0) noexcept;
    bool removeComponent(Entity entId, SceneId id = 0) noexcept;
    bool hasComponent(Entity entId, SceneId id = 0) const noexcept;
    size_t size(SceneId id = 0) const noexcept;
    
    Component& getComponent(Entity entId, const std::function<void(Component&)>& = nullptr, SceneId id = 0);
    const Component& getComponent(Entity entId, const std::function<void(const Component&)>& = nullptr, SceneId id = 0) const;
    Entity findEntity(const std::function<bool(const Component&)>&, SceneId id = 0) noexcept;

    void onRemoveEntity(Entity, SceneId) noexcept override;
    
    friend std::ostream& operator<<(std::ostream& os, const EntityComponentBufferBase<Component>& b);
    friend std::istream& operator>>(std::istream& is, EntityComponentBufferBase<Component>& b);
};
//////////////////////////////////////////////////////////////////////

namespace detail {

	 void* getInstance(const std::type_info& ti);
	 void setInstance(const std::type_info& ti, void*);

	template<typename T> T* getInstanceImpl() {
		auto pInst = reinterpret_cast<T*>(detail::getInstance(typeid(T)));
		if (pInst == nullptr) {
			pInst = new T{};
			detail::setInstance(typeid(T), pInst);
		}
		return pInst;
	}

	template<typename T> void freeInstanceImpl() {
		auto pInst = reinterpret_cast<T*>(detail::getInstance(typeid(T)));
		if (pInst != nullptr) {
			delete pInst;
			detail::setInstance(typeid(T), nullptr);
		}
	}
}

//////////////////////////////////////////////////////////////////////
// EntityComponentBuffer
//
// The buffer classes to be used from the outside
// The base implementation directly inherits from EntityComponentBufferBase and adds
// no other functionality
template<typename Component, typename = void>
class EntityComponentBuffer : public EntityComponentBufferBase<Component> {
public:
    static EntityComponentBuffer<Component>& instance() noexcept {
		//auto pInst = detail::getInstanceImpl<EntityComponentBuffer<Component>>();
        static auto inst = EntityComponentBuffer{};
        return inst;
    }

	~EntityComponentBuffer() {
		std::cout << "Freed buffer " << typeid(*this).name() << "\n";
		//detail::freeInstanceImpl<EntityComponentBuffer<Component>>();
	}
};

// This second implementation is specialized for SaveableEnablerType<Component> = void
// and additionally inherits from SaveableBase and adds the save method
template<typename Component>
class EntityComponentBuffer<Component, typename std::enable_if_t<has_ostream_operator_v<Component> && !IsSavable<Component>, void>> : public EntityComponentBufferBase<Component>, public SaveableBase<Component> {
public:
public:
    static EntityComponentBuffer<Component>& instance() noexcept {
		//auto pInst = detail::getInstanceImpl<EntityComponentBuffer<Component>>();
        static auto inst = EntityComponentBuffer{};
        return inst;
    }

	~EntityComponentBuffer() {
		std::cout << "Freed buffer " << typeid(*this).name() << "\n";
		//detail::freeInstanceImpl<EntityComponentBuffer<Component>>();
	}

    void save(std::ostream& os, Entity e) const override {
        if (EntityComponentBufferBase<Component>::hasComponent(e)) {
            EntityComponentBufferBase<Component>::getComponent(e, [&os](const Component& comp) {
                os << comp << " ";
            });
        }
    }
};

// This third implementation is specialized for SaveableEnablerType<Component> = void
// and additionally inherits fro
template<typename Component>
class EntityComponentBuffer<Component, typename std::enable_if_t<IsSavable<Component>, void>> : public EntityComponentBufferBase<Component>, public SaveableBase<Component> {
public:
public:
    static EntityComponentBuffer<Component>& instance() noexcept {
		//auto pInst = detail::getInstanceImpl<EntityComponentBuffer<Component>>();
        static auto inst = EntityComponentBuffer{};
        return inst;
    }

	~EntityComponentBuffer() {
		std::cout << "Freed buffer " << typeid(*this).name() << "\n";
		//detail::freeInstanceImpl<EntityComponentBuffer<Component>>();
	}

/*

    template<typename C, typename = void>
    struct componentNameSaver {
        void operator()(nlohmann::json&) {}
    };

    template<typename C>
    struct componentNameSaver<C, typename decltype(C::name(), void)> {
        void operator()(nlohmann::json& j) {
            j.emplace("componentName", C::name());
        }
    };
*/

    void save(nlohmann::json& j, Entity e) override {
        if (EntityComponentBufferBase<Component>::hasComponent(e)) {
            EntityComponentBufferBase<Component>::getComponent(e, [&j](Component& comp) {
                nlohmann::json jComp;
                jComp.emplace("componentId", Component::id());
                auto& attribs = jComp["data"];
                auto ser = comp.getSerializables();
                ser.serialize(attribs);
                j.push_back(jComp);
            });
        }
    }
};
///////////////////////////////////////////////////////////////////////

}

#include <System/SystemServices.h>
#include <Entities/EntityManager.h>

namespace Ice {

template<typename Component> EntityComponentBufferBase<Component>::EntityComponentBufferBase() noexcept {
    m_vCompData[0].reserve(MAX_COMPONENTS);
    m_mIndex2Entity[0].reserve(MAX_COMPONENTS);
    m_mEntity2Index[0].reserve(MAX_COMPONENTS);
    entityManager.registerComponentBuffer(this);
}

template<typename Component> bool EntityComponentBufferBase<Component>::addComponent(Entity entId, const Component& comp, SceneId scId) noexcept {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };
    
    typename decltype(m_mEntity2Index)::iterator entIter;
    entIter = m_mEntity2Index.find(scId);

    if (entIter == m_mEntity2Index.end()) {
        m_vCompData[scId].reserve(MAX_COMPONENTS);
        m_mIndex2Entity[scId].reserve(MAX_COMPONENTS);
        m_mEntity2Index[scId].reserve(MAX_COMPONENTS);
        entIter = m_mEntity2Index.find(scId);
    }

    if (entIter->second.find(entId) != entIter->second.end())
        return false;

    auto dataIter = m_vCompData.find(scId);
    dataIter->second.emplace_back(comp);
    m_mIndex2Entity[scId][dataIter->second.size() - 1] = entId;
    entIter->second[entId] = dataIter->second.size() - 1;
    
    return true;
}

template<typename Component> bool EntityComponentBufferBase<Component>::removeComponent(Entity entId, SceneId scId) noexcept {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };
    
    auto entIter = m_mEntity2Index.find(scId);
    if (entIter == m_mEntity2Index.end()) {
        return false;
    }

    if (entIter->second.find(entId) == entIter->second.end())
        return false;

    auto indexIter = m_mIndex2Entity.find(scId);
    auto dataIter = m_vCompData.find(scId);

    const size_t nDeletionIndex = entIter->second[entId];
    const Entity nEntityToMove = indexIter->second[dataIter->second.size() - 1];
    dataIter->second[nDeletionIndex] = std::move(dataIter->second.back()); // Move last element into gap
    entIter->second[nEntityToMove] = nDeletionIndex;
    indexIter->second[nDeletionIndex] = nEntityToMove;

    entIter->second.erase(entId);
    indexIter->second.erase(dataIter->second.size() - 1);
    dataIter->second.pop_back();
    
    return true;
}

template<typename Component> size_t EntityComponentBufferBase<Component>::size(SceneId scId) const noexcept {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };
    return m_vCompData[scId].size();
}

template<typename Component> const Component& EntityComponentBufferBase<Component>::getComponent(Entity entId, const std::function<void(const Component&)>& func, SceneId scId) const {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };
    
    auto entIter = m_mEntity2Index.find(scId);
	if (entIter == m_mEntity2Index.end()) {
		throw std::runtime_error("No such component for this scene");
	}

    if (entIter->second.find(entId) == entIter->second.end())
        throw std::runtime_error("No such component for this entity");
    
    const auto& ret = m_vCompData.at(scId).at(entIter->second.at(entId));
    if (func != nullptr)
        func(ret);

    return ret;
}

template<typename Component> Component& EntityComponentBufferBase<Component>::getComponent(Entity entId, const std::function<void(Component&)>& func, SceneId scId) {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };

    auto entIter = m_mEntity2Index.find(scId);
    if (entIter == m_mEntity2Index.end()) {
        throw std::runtime_error("No such component for this scene");
    }

    if (entIter->second.find(entId) == entIter->second.end())
        throw std::runtime_error("No such component for this entity");

    auto& ret = m_vCompData.at(scId).at(entIter->second.at(entId));
    if (func != nullptr)
        func(ret);

    return ret;
}

template<typename Component> Entity EntityComponentBufferBase<Component>::findEntity(const std::function<bool(const Component&)>& pred, SceneId scId) noexcept {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };

    Entity retEnt{ INVALID_ENTITY_ID };
    
    auto entIter = m_mIndex2Entity.find(scId);
    if (entIter == m_mIndex2Entity.end()) {
        return retEnt;
    }

    const auto& sceneData = m_vCompData[scId];
    for (int i = 0; i < sceneData.size(); ++i) {
        if (pred(sceneData.at(i))) {
            retEnt = entIter->second.at(i);
            break;
        }
    }
    return retEnt;
}

template<typename Component> bool EntityComponentBufferBase<Component>::hasComponent(Entity entId, SceneId scId) const noexcept {
    std::scoped_lock<std::mutex> lck{ m_dataMtx };

    auto entIter = m_mEntity2Index.find(scId);
    if (entIter == m_mEntity2Index.end()) {
        return false;
    }
    
    if (entIter->second.find(entId) == entIter->second.end())
        return false;
    
    return true;
}

template<typename Component> void EntityComponentBufferBase<Component>::onRemoveEntity(Entity entId, SceneId scId) noexcept {
    removeComponent(entId, scId);
}

//template<typename Component>
//void EntityComponentBuffer<Component, typename std::enable_if_t<has_ostream_operator_v<Component>, void>>::save(std::ostream& os, Entity e) const {
//    if (EntityComponentBufferBase<Component>::hasComponent(e)) {
//        const auto& comp = EntityComponentBufferBase<Component>::getComponent(e);
//        os << comp;
//    }
//}

template<typename Component>
SaveableBase<Component, typename std::enable_if_t<has_ostream_operator_v<Component> && !IsSavable<Component>, void>>::SaveableBase() noexcept {
    entityManager.registerSaver(this);
}

template<typename Component>
SaveableBase<Component, typename std::enable_if_t<IsSavable<Component>, void>>::SaveableBase() noexcept {
    entityManager.registerSaver(this);
}

}

#endif /* EntityComponentBuffer_h */
