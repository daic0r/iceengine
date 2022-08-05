//
//  MeshGroupSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 15.04.21.
//  Copyright © 2021 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/MeshGroupSystem.h>
#include <Components/SaveComponent.h>
#include <Components/MeshComponent.h>
#include <Components/Systems/ModelManagerSystem.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/TransformComponent.h>
#include <Components/TagComponent.h>
#include <Components/LinkedEntitiesComponent.h>
#include <Entities/EntityManager.h>
#include <Entities/EntityComponentBuffer.h>
#include <crossguid/guid.hpp>

namespace Ice {

std::vector<Entity> MeshGroupSystem::createInstance(Entity blueprintEnt, const glm::mat4& tr, Ice::Entity addToEntity/* = INVALID_ENTITY_ID*/) {
    auto& meshGroupComp = entityManager.getComponent<MeshGroupComponent>(blueprintEnt);
    meshGroupComp.m_vEntities.clear();

    std::vector<Entity> vRet;
    vRet.reserve(meshGroupComp.m_vMeshIds.size() + 1);
    
    auto retEnt = addToEntity != INVALID_ENTITY_ID ? addToEntity : entityManager.createEntity();
    vRet.push_back(retEnt);
    
    entityManager.addComponent(retEnt, TransformComponent{ tr });
    auto& transformComp = entityManager.getComponent<TransformComponent>(retEnt);
//    entityManager.addComponent(retEnt, SaveComponent{});

	std::string strTagId = xg::newGuid();
//    std::transform(meshGroupComp.m_vMeshIds.cbegin(), meshGroupComp.m_vMeshIds.cend(), std::back_inserter(strTagId), [](const std::string& str) {
//        return str;
//    });

    
    entityManager.addComponent(retEnt, TagComponent{ strTagId });
    
    LinkedEntitiesComponent lec{};
    
    for (const auto& strMeshId : meshGroupComp.m_vMeshIds) {
        auto meshEnt = EntityComponentBuffer<MeshComponent>::instance().findEntity([strMeshId](const MeshComponent& c) {
            return c.name() == strMeshId;
        });
        if (meshEnt == INVALID_ENTITY_ID) {
            vRet.clear();
            return vRet;
        }
        meshGroupComp.m_vEntities.push_back(meshEnt);
        
        auto inst = entityManager.createEntity();
        entityManager.addComponent(inst, SharedComponent<TransformComponent>{ &transformComp, meshEnt, strTagId });
        entityManager.addComponent(inst, ModelReferenceComponent{ meshEnt, strMeshId });
        //entityManager.addComponent(inst, SaveComponent{});
        
        lec.m_sLinkedEntities.insert(inst);
        vRet.push_back(inst);
    }
    
    entityManager.addComponent(retEnt, lec);
    
    return vRet;
}

}
