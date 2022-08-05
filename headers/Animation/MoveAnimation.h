//
//  MoveAnimation.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MoveForwardAnimation_h
#define MoveForwardAnimation_h

#include "Animation.h"
#include <glm/vec3.hpp>

namespace Ice {

class MoveAnimation :
    public Animation
{
    glm::vec3 m_position;
    glm::vec3 m_direction;
    float m_fAmount;
    float m_fDelta;
    glm::vec3* m_pTarget{ nullptr };
    bool m_bDone{ false };

public:
    MoveAnimation(glm::vec3*, const glm::vec3&, float);

    void animate(int nCurrentTime) override;
    bool done() override;
};

}

#endif /* MoveForwardAnimation_h */
