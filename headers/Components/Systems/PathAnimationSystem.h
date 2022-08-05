//
//  PathAnimationSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathAnimationSystem_h
#define PathAnimationSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/PathAnimationComponent.h>
#include <vector>

namespace Ice {

class PathAnimationSystem : public EntityComponentSystem<PathAnimationComponent> {
    std::vector<Entity> m_vDoneEntities;

public:
    
    bool update(float fDeltaTime) override;
};

}

#endif /* PathAnimationSystem_h */
