//
//  ModelAnimationComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelAnimationComponent_h
#define ModelAnimationComponent_h

#include <Components/ComponentIds.h>
#include <ModelAnimation/ModelAnimation.h>
#include <Utils/serializables.h>

namespace Ice {

struct ModelAnimationComponent {
  
    static constexpr EntityComponentId id() noexcept { return MODEL_ANIMATION_COMPONENT; }
    
    ModelAnimation m_animation;

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(modelAnimation, m_animation)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

}

#endif /* ModelAnimationComponent_h */
