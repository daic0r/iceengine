//
//  PathSegmentBlueprintSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 12.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Entities/EntityManager.h>

#include <Components/Systems/PathSegmentBlueprintSystem.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/TransformComponent.h>
#include <Components/GraphNodeComponent.h>
#include <Components/SaveComponent.h>
#include <Components/Systems/ModelManagerSystem.h>
#include <Components/Systems/MeshGroupSystem.h>
#include <glm/mat4x4.hpp>

namespace Ice {

Ice::Entity PathSegmentBlueprintSystem::createInstance(Ice::Entity blueprintEnt, const glm::mat4& tr, Ice::Entity addToEntity/* = INVALID_ENTITY_ID*/) const {
    using Ice::SystemServices;

    Entity retEnt = INVALID_ENTITY_ID;
    if (entityManager.hasComponent<MeshComponent>(blueprintEnt)) {
        auto pMMS = entityManager.getSystem<Ice::ModelManagerSystem, false>();
        retEnt = pMMS->createInstance(blueprintEnt, tr, addToEntity);
        
        auto pathSeg = entityManager.getComponent<Ice::PathSegmentComponent>(blueprintEnt);
        entityManager.addComponent(retEnt, pathSeg);
        if (entityManager.hasComponent<Ice::GraphNodeComponent>(blueprintEnt)) {
            entityManager.addComponent(retEnt, entityManager.getComponent<Ice::GraphNodeComponent>(blueprintEnt));
        }
    } else {
        auto pMGS = entityManager.getSystem<Ice::MeshGroupSystem, false>();
        auto vEnts = pMGS->createInstance(blueprintEnt, tr, addToEntity);
        
        const auto& groupComp = entityManager.getComponent<MeshGroupComponent>(blueprintEnt);
        
        for (size_t i = 0; i < groupComp.m_vEntities.size(); ++i) {
            
            auto sourceEnt = groupComp.m_vEntities.at(i);
            auto instEnt = vEnts.at(i + 1);
            
            auto pathSeg = entityManager.getComponent<Ice::PathSegmentComponent>(sourceEnt);
            entityManager.addComponent(instEnt, pathSeg);
            if (entityManager.hasComponent<Ice::GraphNodeComponent>(sourceEnt)) {
                entityManager.addComponent(instEnt, entityManager.getComponent<Ice::GraphNodeComponent>(sourceEnt));
            }
        }
        retEnt = vEnts.front();
    }

	return retEnt;
}

}
