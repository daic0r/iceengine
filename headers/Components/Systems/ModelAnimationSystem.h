//
//  ModelAnimationSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelAnimationSystem_h
#define ModelAnimationSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/SkeletonComponent.h>
#include <Components/ModelAnimationComponent.h>

namespace Ice {

class Joint;
class ModelAnimation;

class ModelAnimationSystem : public EntityComponentSystem<ModelAnimationComponent, SkeletonComponent> {
    
    void updateJoint(Joint&, const ModelAnimation&, const glm::mat4& parentTransform);
    
public:
    bool update(float fDeltaTime) override;
};

}

#endif /* ModelAnimationSystem_h */
