//
//  Particle.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Particle_h
#define Particle_h

#include <glm/vec3.hpp>
#include <string>

namespace Ice {

class BaseTexture;

class Particle {
    static constexpr float GRAVITY = -50.0f;
    
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_fGravityEffect;
    float m_fLifeLength;
    float m_fRotation;
    float m_fScale;
    float m_fElapsedTime{ 0.0f };
    BaseTexture *m_pTexture { nullptr };
    std::pair<float, float> m_texOffset1, m_texOffset2;
    int m_fNumTextureStages;
    int m_nCurrentStage{ 0 };
    float m_fBlendFactor;
    
public:
    
    Particle(const glm::vec3& pos, const glm::vec3& vel, float fGravityEffect, float fLifeLength, float fRotation, float fScale, BaseTexture *pTex);
    const auto& position() const noexcept { return m_position; }
    void setPosition(const glm::vec3& pos) noexcept { m_position = pos; }
    auto scale() const noexcept { return m_fScale; }
    void setScale(float fScale) noexcept { m_fScale = fScale; }
    auto rotation() const noexcept { return m_fRotation; }
    void setRotation(float fRotation) noexcept { m_fRotation = fRotation; }
    auto texture() const noexcept { return m_pTexture; }
    void setTexture(BaseTexture *pTex) noexcept { m_pTexture = pTex; }
    auto numTextureStages() const noexcept { return m_fNumTextureStages; }
    void setNumTextureStages(int nNum) noexcept { m_fNumTextureStages = nNum; }
    const auto& texOffset1() const noexcept { return m_texOffset1; }
    const auto& texOffset2() const noexcept { return m_texOffset2; }
    auto blendFactor() const noexcept { return m_fBlendFactor; }

    
    
    bool update(float fDeltaTime) noexcept;
    void updateTextureData(float fDeltaTime) noexcept;
    
};

}

#endif /* Particle_h */
