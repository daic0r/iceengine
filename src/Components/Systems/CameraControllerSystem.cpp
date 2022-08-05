//
//  CameraControllerSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/CameraControllerSystem.h>
#include <Entities/EntityManager.h>
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <System/SystemServices.h>
#include <Interfaces/IMasterRenderer.h>
#include <Components/CameraComponent.h>
#include <System/Tools.h>
#include <iostream>
#include <Interfaces/IGraphicsSystem.h>
#include <Events/EventQueue.h>
#include <Events/KeyDownEventArgs.h>
#include <Events/MouseWheelEventArgs.h>
#include <Components/Systems/TerrainSystem.h>

namespace Ice {

CameraControllerSystem::CameraControllerSystem() {
	m_pGraphicsSystem = systemServices.getGraphicsSystem();
}

bool CameraControllerSystem::update(float fDeltaTime) {
    static constexpr float fMargin = 30.0f;

    Entity e = *entities(entityManager.currentScene()).begin();
    auto &comp = entityManager.getComponent<CameraComponent>(e);
    int x, y;
    auto buttonState = SDL_GetRelativeMouseState(&x, &y);
    bool bMiddleButton = buttonState & SDL_BUTTON(SDL_BUTTON_MIDDLE);
    
    auto& camera = comp.m_camera;

    if (bMiddleButton) {
        camera.incYawAndPitch(x / 50.0f, 0.0f);
    }

    while (auto pEvent = systemServices.getEventQueue()->peekInternalEvent()) {
        switch (pEvent->id()) {            
            case EventId::KEY_DOWN_EVENT:
            {
                const auto& args = std::any_cast<const KeyDownEventArgs&>(pEvent->args());
                switch (args.key()) {
                    case SDLK_w:
                        camera.moveForward(5.0f);
                        break;
                    case SDLK_s:
                        camera.moveForward(-5.0f);
                        break;
                    case SDLK_a:
                        camera.moveSideways(-5.0f);
                        break;
                    case SDLK_d:
                        camera.moveSideways(5.0f);
                        break;
                    case SDLK_q:
                        camera.incYawAndPitch(-M_PI / 4.0f, 0.0f);
                        break;
                    case SDLK_e:
                        camera.incYawAndPitch(M_PI / 4.0f, 0.0f);
                        break;
                    case SDLK_z:
                        camera.zoom(-1.0f);
                        break;
                    case SDLK_x:
                        camera.zoom(1.0f);
                        break;
                }
            }
            break;
            case EventId::MOUSE_WHEEL_EVENT:
            {
                const auto& args = std::any_cast<const MouseWheelEventArgs&>(pEvent->args());
                camera.zoom(-args.y());
            }
            break;
        }
    }
    
    camera.update(fDeltaTime);
    if (m_pTerrainSystem == nullptr)
        m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
    if (!camera.hasHeightGetterFunc())
        camera.setHeightGetterFunc([this](float x, float z) {
            return m_pTerrainSystem->getHeight(x, z).value_or(100.0f);
        });
	m_frustum = Frustum{ camera, m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distFarPlane(), glm::radians(m_pGraphicsSystem->fov()), m_pGraphicsSystem->aspectRatio() };

    return true;
}

}
