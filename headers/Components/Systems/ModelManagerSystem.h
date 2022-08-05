//
//  ModelManagerSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelManagerSystem_h
#define ModelManagerSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <glm/mat4x4.hpp>

namespace Ice {

struct Model;

class ModelManagerSystem : public EntityComponentSystem<MeshComponent, RenderMaterialsComponent> {
    IModelManager *m_pManager{ nullptr };
    
    void onEntityAdded(Entity) noexcept override;
    void willRemoveComponent(Entity, EntityComponentId) noexcept override;
    void modelFromComponents(Entity, Model&) noexcept;
public:
    
    ModelManagerSystem() noexcept;
    Entity createInstance(Entity, const glm::mat4& = glm::mat4{ 1.0f }, Ice::Entity addToEntity = INVALID_ENTITY_ID);

};

}

#endif /* ModelManagerSystem_h */
