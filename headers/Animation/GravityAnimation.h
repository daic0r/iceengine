//
//  GravityAnimation.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef GravityAnimation_h
#define GravityAnimation_h

#include "Animation.h"
#include <glm/vec3.hpp>

namespace Ice {

class GravityAnimation :
    public Animation
{
    glm::vec3 m_fromPosition;
    glm::vec3 m_toPosition;
    glm::vec3* m_pTarget{ nullptr };
    glm::vec3 m_direction;
    float m_fDelta;
    bool m_bDone { false };

public:
    GravityAnimation(glm::vec3*, const glm::vec3&, const glm::vec3&);

    void animate(int nCurrentTime) override;
    bool done() override;
};

}

#endif /* GravityAnimation_h */
