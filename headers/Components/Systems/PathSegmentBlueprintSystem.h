//
//  PathSegmentBlueprintSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 12.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathSegmentBlueprintSystem_h
#define PathSegmentBlueprintSystem_h

#include <IceEngine.h>
#include <Entities/EntityComponentSystem.h>
#include <Components/PathSegmentComponent.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <glm/mat4x4.hpp>

namespace Ice {

class PathSegmentBlueprintSystem : public Ice::EntityComponentSystem<Ice::MeshComponent, Ice::RenderMaterialsComponent, Ice::PathSegmentComponent> {
    
public:
    Ice::Entity createInstance(Ice::Entity, const glm::mat4& = glm::mat4{ 1.0f }, Ice::Entity addToEntity = INVALID_ENTITY_ID) const;
};

}

#endif /* PathSegmentBlueprintSystem_h */
