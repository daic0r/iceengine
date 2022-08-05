//
//  Particle.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Particles/Particle.h>
#include <math.h>
#include <iostream>
#include <System/BaseTexture.h>

namespace Ice {

Particle::Particle(const glm::vec3& pos, const glm::vec3& vel, float fGravityEffect, float fLifeLength, float fRotation, float fScale, BaseTexture* pTexture)
: m_position{ pos },
m_velocity{ vel },
m_fGravityEffect{ fGravityEffect },
m_fLifeLength{ fLifeLength },
m_fRotation{ fRotation },
m_fScale{ fScale },
m_pTexture{ pTexture }
{
    m_fNumTextureStages = texture()->rows() * texture()->cols();
}

bool Particle::update(float fDeltaTime) noexcept {
    m_velocity.y += GRAVITY * m_fGravityEffect * fDeltaTime;
    m_position += m_velocity * fDeltaTime;
    m_fElapsedTime += fDeltaTime;
    bool bLifeOver = m_fElapsedTime < m_fLifeLength;
    updateTextureData(fDeltaTime);
    return bLifeOver;
}

void Particle::updateTextureData(float fDeltaTime) noexcept {
    if (texture() == nullptr)
        return;
    
    float fCurrentStage = (m_fElapsedTime / m_fLifeLength) * m_fNumTextureStages;
    
    int nCol1 = static_cast<int>(fCurrentStage) % texture()->cols();
    int nRow1 = static_cast<int>(fCurrentStage) / texture()->cols();
    
    int nCol2 = static_cast<int>(fCurrentStage) < static_cast<int>(m_fNumTextureStages)-1 ? (static_cast<int>(fCurrentStage)+1) % texture()->cols() : nCol1;
    int nRow2 = static_cast<int>(fCurrentStage) < static_cast<int>(m_fNumTextureStages)-1 ? (static_cast<int>(fCurrentStage)+1) / texture()->cols() : nRow1;
    
    m_texOffset1 = texture()->getTexCoord(nCol1 * (texture()->width() / texture()->cols()), (nRow1 + 1) * (texture()->height() / texture()->rows()));
    m_texOffset2 = texture()->getTexCoord(nCol2 * (texture()->width() / texture()->cols()), (nRow2 + 1) * (texture()->height() / texture()->rows()));
    
    m_fBlendFactor = fCurrentStage - floor(fCurrentStage);
    
//    if (ptr == this)
//    std::cout << "Tex stage: " << fCurrentStage << ", [" << nRow1 << ", " << nCol1 << "]" << ", [" << m_texOffset1.first << ", " << m_texOffset1.second << "]" << std::endl;
}

}
