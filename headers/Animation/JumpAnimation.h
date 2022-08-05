#pragma once

#include "Animation.h"
#include <glm/vec3.hpp>

namespace Ice {

class JumpAnimation :
    public Animation
{
    glm::vec3 m_position;
    glm::vec3* m_pTarget{ nullptr };
    bool m_bDone{ false };

public:
    JumpAnimation(glm::vec3*);

    void animate(int nCurrentTime) override;
    bool done() override;
};

}
