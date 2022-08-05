//
//  ParticleSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 11.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Particles/ParticleSystem.h>
#include <Particles/Particle.h>
#include <random>
//#include "../Interfaces/ITextureManager.h"
#include <System/SystemServices.h>
#include <functional>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdexcept>
#include <iostream>

namespace Ice {

ParticleSystem::ParticleSystem(const glm::vec3& pos, int nPps, float fGravity, float fLifeLength, float fSpeed, BaseTexture* pTexture)
: m_position{ pos },
m_nPPS{ nPps },
m_fGravityEffect{ fGravity },
m_fLifeLength { fLifeLength },
m_fSpeed{ fSpeed },
m_pTexture{ pTexture }
{
    m_fPPSCarry = 0.0f;
}

void ParticleSystem::update(float fDeltaTime) noexcept {
	for (auto iter = m_vParticles.begin(); iter != m_vParticles.end(); ++iter) {
		auto& particle = *iter;
        if (!particle.update(fDeltaTime)) {
			iter = m_vParticles.erase(iter);
            //std::cout << "Disposed particle" << std::endl;
        }
    }

    float fNumCreate = fDeltaTime * static_cast<float>(m_nPPS);
    int nNumCreate = static_cast<int>(fNumCreate);
    m_fPPSCarry += fNumCreate - floor(fNumCreate);
    if (m_fPPSCarry >= 1.0f) {
        ++nNumCreate;
        m_fPPSCarry = 0.0f;
    }
    for (int i = 0; i < nNumCreate; ++i)
        emitSingle();


}

void ParticleSystem::emitSingle() noexcept {
    static std::random_device rd;
    static auto gen = std::bind(std::uniform_real_distribution<float>(-1.0f, 1.0f),
                               std::mt19937(rd()));

    
    glm::vec3 velocity{ gen(), 1, gen() };
    velocity = glm::normalize(velocity) * m_fSpeed;
    float fRot = (gen() + 1.0f) / 2.0f;
    fRot *= 2*M_PI_2;
	m_vParticles.emplace_back(m_position, velocity, m_fGravityEffect, m_fLifeLength, fRot, ((gen() + 1.0f) / 2.0f), texture());
    //auto pParticle = std::make_shared<Particle>( m_position, velocity, m_fGravityEffect, m_fLifeLength, fRot, ((gen() + 1.0f) / 2.0f), texture() );
    //ParticleManager::instance().addParticle(pParticle);
}

//void ParticleSystem::setTextureId(const std::string &strTextureId) {
//    m_strTextureId = strTextureId;
//    //m_pTexture = textureManager->getTextureAtlas(strTextureId);
//}

}
