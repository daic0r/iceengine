//
//  AnimatedModelInstance.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedModelInstance_h
#define AnimatedModelInstance_h

#include <System/ModelInstance.h>
#include <vector>
#include <glm/mat4x4.hpp>

namespace Ice {

struct AnimatedModelInstance : public ModelInstance {

    std::vector<glm::mat4> vJointTransforms;
};

}

#endif /* AnimatedModelInstance_h */
