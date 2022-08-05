#pragma once

#include <IceEngine.h>
#include <string>
#include <Entities/Entity.h>
#include <map>
#include <functional>
#include <memory>
#include <Entities/EntityManager.h>
#include <System/SystemServices.h>
#include <Interfaces/IComponentManager.h>
#include <nlohmann/json.hpp>

namespace Ice {

struct BaseComponent;

class ICEENGINE_API ComponentManager : public IComponentManager
{

	std::unique_ptr<BaseComponent> createComponent();
	std::map<EntityComponentId, std::function<bool(Entity, std::istream&)>> m_mLoadComponentFuncs;
	std::map<EntityComponentId, std::function<void(Entity, const nlohmann::json&)>> m_mCreateCompFuncs;


protected:
	template<typename Component> static bool loadComponent(Entity e, std::istream& is)
	{
		Component comp;
		try {
			is >> comp;
		}
		catch (...) {
			return false;
		}
		entityManager.addComponent<Component>(e, comp);
		return true;
	}

	virtual bool subLoadComponent(Entity, EntityComponentId, std::istream&) const { return false; }
	virtual bool subLoadComponent(Entity, EntityComponentId, const nlohmann::json&) const { return false; }

	template<typename Component>
	static void addAndDeserialize(Entity e, const nlohmann::json& j) {
		entityManager.addComponent(e, Component{});
		auto& comp = entityManager.getComponent<Component>(e);
		auto ser = comp.getSerializables();
		ser.deserialize(j);
	}

public:

	ComponentManager();
	std::vector<Entity> loadEntityFile(const std::string&) const override;
	std::vector<Entity> loadEntityFileJson(const std::string&) const override;
};

}
