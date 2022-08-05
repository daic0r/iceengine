//
//  ModelReference.h
//  IceCraft
//
//  Created by Matthias Gruen on 07.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelReference_h
#define ModelReference_h

#include <Components/TransformComponent.h>

namespace Ice {

struct ModelInstance {
    TransformComponent* pTransform{ nullptr };
};

}

#endif /* GLModelReference_h */
