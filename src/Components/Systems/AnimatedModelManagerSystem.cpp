//
//  AnimatedModelManagerSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 22.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/AnimatedModelManagerSystem.h>
#include <Interfaces/IModelRenderer.h>
#include <System/SystemServices.h>
#include <System/AnimatedModel.h>
#include <Entities/EntityManager.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <Components/AnimatedMeshComponent.h>
#include <Interfaces/IModelManager.h>

namespace Ice {
    
AnimatedModelManagerSystem::AnimatedModelManagerSystem() noexcept {
    m_pManager = systemServices.getModelManager();
}

void AnimatedModelManagerSystem::onEntityAdded(Entity e) noexcept {
    AnimatedModel mod;
    modelFromComponents(e, mod);
    
    m_pManager->registerModel(&mod);
}

void AnimatedModelManagerSystem::willRemoveComponent(Entity e, EntityComponentId) noexcept {
    AnimatedModel mod;
    modelFromComponents(e, mod);

    m_pManager->unregisterModel(&mod);
}

void AnimatedModelManagerSystem::modelFromComponents(Entity e, AnimatedModel& mod) noexcept {
    auto& mesh = entityManager.getComponent<MeshComponent>(e);
    auto& materials = entityManager.getComponent<RenderMaterialsComponent>(e);
    auto& aniMesh = entityManager.getComponent<AnimatedMeshComponent>(e);

    mod.pMesh = &mesh;
    mod.pMaterials = &materials;
    mod.pAnimatedMesh = &aniMesh;
}

}
