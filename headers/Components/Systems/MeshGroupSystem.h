//
//  MeshGroupSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.04.21.
//  Copyright © 2021 Matthias Gruen. All rights reserved.
//

#ifndef MeshGroupSystem_h
#define MeshGroupSystem_h

#include <Components/ComponentIds.h>
#include <Entities/EntityComponentSystem.h>
#include <Components/MeshGroupComponent.h>
#include <Entities/Entity.h>
#include <glm/mat4x4.hpp>

namespace Ice {

class MeshGroupSystem : public EntityComponentSystem<MeshGroupComponent> {
public:
    std::vector<Entity> createInstance(Entity, const glm::mat4& = glm::mat4{ 1.0f }, Ice::Entity addToEntity = INVALID_ENTITY_ID);
};

}

#endif /* MeshGroupSystem_h */
