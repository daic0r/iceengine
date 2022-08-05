//
//  ParticleSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 11.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ParticleSystem_h
#define ParticleSystem_h

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <vector>
#include <Particles/Particle.h>

namespace Ice {

class ParticleSystem {
    glm::vec3 m_position;
    int m_nPPS;
    float m_fGravityEffect;
    float m_fLifeLength;
    float m_fSpeed;
    float m_fPPSCarry;
	std::vector<Particle> m_vParticles;
    BaseTexture *m_pTexture{ nullptr };
        
public:
    ParticleSystem(const glm::vec3& pos, int nPps, float fGravity, float fLifeLength, float fSpeed, BaseTexture *pTexture);

	const auto& position() const noexcept { return m_position; }
	void setPosition(const glm::vec3& pos) noexcept { m_position = pos; }
	auto& particles() noexcept { return m_vParticles; }
	const auto& particles() const noexcept { return m_vParticles; }
    //const std::string& textureFile() const noexcept { return m_strTextureFile; }
    //void setTextureFile(const std::string& strTextureId);
    auto texture() const noexcept { return m_pTexture; }
    void setTexture(BaseTexture *pTex) noexcept { m_pTexture = pTex; }
    
    void emitSingle() noexcept;
    void update(float fDeltaTime) noexcept;
};


}

#endif /* ParticleSystem_h */
