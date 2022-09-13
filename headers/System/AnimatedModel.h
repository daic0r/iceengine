//
//  AnimatedModel.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedModel_h
#define AnimatedModel_h

#include <System/Model.h>
#include <Components/AnimatedMeshComponent.h>

namespace Ice {

struct AnimatedModel : public Model {
  
    AnimatedMeshComponent* pAnimatedMesh{ nullptr };
    size_t m_nNumJoints{ 0 };
};

}

namespace std {
    template<>
    struct hash<Ice::AnimatedModel> {
        size_t operator()(const Ice::AnimatedModel& m) const {
            return std::hash<Ice::MeshComponent*>{}(m.pMesh);
        }
    };
}

#endif /* AnimatedModel_h */
