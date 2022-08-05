//
//  SkyboxRenderingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/SkyboxRenderingSystem.h>
#include <System/SystemServices.h>
#include <Interfaces/ISkyboxRenderer.h>
#include <Entities/EntityManager.h>
#include <Components/SkyboxComponent.h>
#define _USE_MATH_DEFINES
#include <math.h>

namespace Ice {

SkyboxRenderingSystem::SkyboxRenderingSystem() noexcept {
    m_pRenderer = systemServices.getSkyboxRenderer();
}

void SkyboxRenderingSystem::render(const RenderEnvironment &env) noexcept {
    if (entities(entityManager.currentScene()).size() > 0)
        m_pRenderer->render(env, m_fRotation);
}

bool SkyboxRenderingSystem::update(float fDeltaTime) noexcept {
	m_fRotation += fDeltaTime * .05f;
	m_fRotation = fmodf(m_fRotation, 2 * M_PI);
	return true;
}

}
