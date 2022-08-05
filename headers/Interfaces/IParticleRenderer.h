//
//  IParticleRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IParticleRenderer_h
#define IParticleRenderer_h

#include <vector>
#include <glm/mat4x4.hpp>
#include <unordered_map>
#include "ITexture.h"

namespace Ice {

struct RenderEnvironment;
class Particle;
class ITexture;
class ParticleSystem;

class IParticleRenderer {
public:
    
    virtual const glm::mat4& projectionMatrix() const noexcept = 0;
    virtual void setProjectionMatrix(const glm::mat4& proj) noexcept = 0;
    
    virtual void render(const RenderEnvironment&, std::vector<ParticleSystem*>&) noexcept = 0;
    virtual ~IParticleRenderer() {}
};

}

#endif /* IParticleRenderer_h */
