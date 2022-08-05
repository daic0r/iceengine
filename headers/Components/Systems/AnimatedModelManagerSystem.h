//
//  AnimatedModelManagerSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedModelManagerSystem_h
#define AnimatedModelManagerSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <Components/AnimatedMeshComponent.h>

namespace Ice {

struct AnimatedModel;

// Entities with these 3 components are automatically added to this system's list of entities
// Thus it functions as a container for all objects with these 3 components, which constitute an animated model
class AnimatedModelManagerSystem : public EntityComponentSystem<MeshComponent, AnimatedMeshComponent, RenderMaterialsComponent> {
    IModelManager *m_pManager{ nullptr };
    
    void onEntityAdded(Entity) noexcept override;
    void willRemoveComponent(Entity, EntityComponentId) noexcept override;
    void modelFromComponents(Entity, AnimatedModel&) noexcept;
    
public:
    AnimatedModelManagerSystem() noexcept;
    
};

}

#endif /* AnimatedModelManagerSystem_h */
