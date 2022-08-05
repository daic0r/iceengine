//
//  ModelManagerSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/ModelManagerSystem.h>
#include <Interfaces/IModelManager.h>
#include <System/SystemServices.h>
#include <System/Model.h>
#include <Entities/EntityManager.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <Components/AnimatedMeshComponent.h>
#include <Components/TransformComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/SaveComponent.h>
#include <Components/ModelInstanceTagComponent.h>

namespace Ice {

ModelManagerSystem::ModelManagerSystem() noexcept {
    m_pManager = systemServices.getModelManager();
}

void ModelManagerSystem::onEntityAdded(Entity e) noexcept {
    if (entityManager.hasComponent<AnimatedMeshComponent>(e)) {
        return;
    }
    
    Model mod;
    modelFromComponents(e, mod);
   
    m_pManager->registerModel(&mod);
}

void ModelManagerSystem::willRemoveComponent(Entity e, EntityComponentId) noexcept {
    Model mod;
    modelFromComponents(e, mod);

    m_pManager->unregisterModel(&mod);
}

void ModelManagerSystem::modelFromComponents(Entity e, Model& mod) noexcept {
    auto& mesh = entityManager.getComponent<MeshComponent>(e);
    auto& materials = entityManager.getComponent<RenderMaterialsComponent>(e);
    
    mod.pMesh = &mesh;
    mod.pMaterials = &materials;
}

Entity ModelManagerSystem::createInstance(Entity blueprintEnt, const glm::mat4& tr, Ice::Entity addToEntity/* = INVALID_ENTITY_ID*/) {
   
    auto retEnt = addToEntity != INVALID_ENTITY_ID ? addToEntity : entityManager.createEntity();
    entityManager.addComponent(retEnt, Ice::TransformComponent{ tr });
    entityManager.addComponent(retEnt, Ice::ModelReferenceComponent{ blueprintEnt });
	entityManager.addComponent(retEnt, Ice::ModelInstanceTagComponent{});
    //entityManager.addComponent(retEnt, Ice::SaveComponent{});

    return retEnt;
}

}
