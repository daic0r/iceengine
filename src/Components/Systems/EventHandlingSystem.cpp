//
//  EventHandlingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/EventHandlingSystem.h>
#include <Entities/EntityManager.h>
#include <Components/InputReceiverComponent.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>
#include <Interfaces/IEventQueue.h>
#include <Events/EventId.h>
#include <Events/MouseMotionEventArgs.h>
#include <Events/EventQueue.h>
#include <Events/MouseButtonEventArgs.h>
#include <Events/WindowResizedEvent.h>
#include <Events/TextInputEventArgs.h>
#include <Events/KeyDownEventArgs.h>
#include <Events/MouseWheelEventArgs.h>
#include <GUI/WidgetManager.h>
#include <Components/Systems/ObjectRenderingSystem.h>
#include <Components/Systems/AnimatedModelRenderingSystem.h>
#include <Components/Systems/SceneGraphSystem.h>
#include <Components/ModelInstanceTagComponent.h>
#include <Components/CameraComponent.h>
#include <GUI/MousePicker.h>
#include <System/AABB.h>
#include <System/Ray.h>
#include <ranges>

namespace Ice {

EventHandlingSystem::EventHandlingSystem() {
}

bool EventHandlingSystem::isModelInstanceUnderMouse(Entity e, const Ray& mouseRay, const CameraComponent& camComp) const noexcept {
    const auto& modRef = entityManager.getComponent<ModelReferenceComponent>(e);
    const auto& meshComp = entityManager.getComponent<MeshComponent>(modRef.m_entity);
    const auto ext = meshComp.extents();
    const auto& transComp = entityManager.getSharedComponentOr<TransformComponent>(e);
    AABB box{ ext };
    box = box.transform(transComp.m_transform);
    const bool bIntersects = box.intersects(mouseRay);
    return bIntersects;
}

bool EventHandlingSystem::update(float fDeltaTime) {
    
    SDL_Event event;
    bool bRet{ true };
    
    auto invokeFunc = [&event, &bRet, this](bool bIsKeyboard) {
		if (bIsKeyboard) {
			// Everyone with a keyboard handler will receive keyboard events
			for (auto e : entities(entityManager.currentScene())) {
				auto& comp = entityManager.getComponent<InputReceiverComponent>(e);
				for (const auto& func : comp.m_vKeyboardHandlers)
					bRet &= func(e, event.type, &event.key);
			}
		} else {
			const auto& ents = entities(entityManager.currentScene());
            /*
            const auto& sFrustumEnts1 = m_pObjRenderingSystem->entitiesInFrustum();
			const auto& sFrustumEnts2 = m_pAnimObjRenderingSystem->entitiesInFrustum();
            auto checkEnts = ents | std::views::filter([&sFrustumEnts1,&sFrustumEnts2](auto e) {
                const auto bHasTransform = entityManager.hasComponent<TransformComponent>(e);
                return !bHasTransform || (bHasTransform && (sFrustumEnts1.contains(e) || sFrustumEnts2.contains(e)));
            });
            */
			// Mouse events will only be received by entities that are in view
			// Otherwise they can't be accessed with the mouse
			//m_vEntBuffer.clear();
			// Requires running the 2 rendering systems' update methods before this one!
			// Ensure via exceutionOrder()!
			//const auto& sFrustumEnts1 = m_pObjRenderingSystem->entitiesInFrustum();
			//const auto& sFrustumEnts2 = m_pAnimObjRenderingSystem->entitiesInFrustum();
			//const auto& ents = entities(entityManager.currentScene());
			//std::set_intersection(sFrustumEnts1.cbegin(), sFrustumEnts1.cend(), ents.cbegin(), ents.cend(), std::back_inserter(m_vEntBuffer));
			//std::set_intersection(sFrustumEnts2.cbegin(), sFrustumEnts2.cend(), ents.cbegin(), ents.cend(), std::back_inserter(m_vEntBuffer));
            const auto& camComp = entityManager.getComponent<CameraComponent>(cameraEnt());
            int x, y;
            // TODO: Replace direct access by something like IMouseInputController
            // and IKeyboardInputController which can be fed by anything
            SDL_GetMouseState(&x, &y);
            const MousePicker pick{ x, y, camComp.m_camera.matrix() } ;
            const Ray mouseRay{ camComp.m_camera.position(), pick.getMouseRay() };

            m_vEntBuffer.clear(); 
            m_pSceneGraphSystem->tree().intersects(mouseRay, [this](const SceneGraphSystem::TreeNodeContainer& container) {
                this->m_vEntBuffer.insert(this->m_vEntBuffer.end(), container.m_vObjects.begin(), container.m_vObjects.end());
                return SubdivisionIntersectionBehavior::ABORT_SUCCESS; 
            }); //, m_vEntBuffer);
            std::ranges::sort(m_vEntBuffer);

            auto checkEnts = ents | std::views::filter([this](auto e) {
                const auto bHasTransform = entityManager.hasComponent<TransformComponent>(e);
                return !bHasTransform || (bHasTransform && std::ranges::binary_search(m_vEntBuffer, e));
            });
 			for (auto e : checkEnts) {
				auto& comp = entityManager.getComponent<InputReceiverComponent>(e);
                for (const auto& func : comp.m_vMouseHandlers) {
                    const auto bHasComp = entityManager.hasComponent<ModelInstanceTagComponent>(e);
                    const auto bInvoke = !bHasComp || (bHasComp &&
                             isModelInstanceUnderMouse(e, mouseRay, camComp));
                    if (bInvoke)
                        bRet &= func(e, event.type, &event.motion, &event.button, &event.wheel);
                }
            }
        }
    };

    while (SDL_PollEvent(&event)) {
            
        switch (event.type){
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        systemServices.getGraphicsSystem()->resizeWindow(event.window.data1, event.window.data2);
                        systemServices.getEventQueue()->queueInternalEvent(Event{ EventId::WINDOW_RESIZED_EVENT, WindowResizedEventArgs{ event.window.data1, event.window.data2 } });
                        break;
                }
                break;
            case SDL_QUIT:
                return false;
            case SDL_TEXTINPUT:
            {
                auto chArray = event.text.text;
                systemServices.getEventQueue()->queueInternalEvent(Event{ EventId::TEXT_INPUT_EVENT, TextInputEventArgs{ chArray[0] } });
            }
                break;
            case SDL_KEYUP:
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_BACKSPACE ||
                    event.key.keysym.sym == SDLK_ESCAPE ||
                    event.key.keysym.sym == SDLK_RETURN) {
                    auto pEvent = Event{ EventId::TEXT_INPUT_EVENT, TextInputEventArgs{} };
                    auto& args = std::any_cast<TextInputEventArgs&>(pEvent.args());
                    args.m_bBackspace = event.key.keysym.sym == SDLK_BACKSPACE;
                    args.m_bEscape = event.key.keysym.sym == SDLK_ESCAPE;
                    args.m_bEnter = event.key.keysym.sym == SDLK_RETURN;
                    systemServices.getEventQueue()->queueInternalEvent(std::move(pEvent));
                }
                if (SDL_IsTextInputActive() == SDL_FALSE) {
                    auto pEvent = Event{ EventId::KEY_DOWN_EVENT, KeyDownEventArgs(event.key.keysym.sym) };
                    systemServices.getEventQueue()->queueInternalEvent(std::move(pEvent));
                    invokeFunc(true);
                }
                break;
            case SDL_MOUSEWHEEL:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                if (event.type == SDL_MOUSEMOTION)
                    systemServices.getEventQueue()->queueInternalEvent(Event{ EventId::MOUSE_MOTION_EVENT, MouseMotionEventArgs{ event.motion } });
                else
                if (event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEBUTTONDOWN)
                    systemServices.getEventQueue()->queueInternalEvent(Event{ EventId::MOUSE_BUTTON_EVENT, MouseButtonEventArgs{ event.button } });
                else 
                if (event.type == SDL_MOUSEWHEEL)
                    systemServices.getEventQueue()->queueInternalEvent(Event{ EventId::MOUSE_WHEEL_EVENT, MouseWheelEventArgs{ event.wheel.x, event.wheel.y } });
                // Only notify game entities if we're not over a GUI element!
                if (!systemServices.getWidgetManager().isMouseOverGUI())
                    invokeFunc(false);
                break;
        }
    }
    
    return bRet;
}

void EventHandlingSystem::onSystemsInitialized() noexcept {
    m_pSceneGraphSystem = entityManager.getSystem<SceneGraphSystem, true>(); 
    
    /*
    m_pObjRenderingSystem->kdTree().setIntersectsCollectionFunc([this](const Ray&, const ObjectRenderingSystem::ModelRenderingKdTreeNodeContainer& container) {
        this->m_vEntBuffer.insert(this->m_vEntBuffer.end(), container.m_vObjects.begin(), container.m_vObjects.end());
        return !container.m_vObjects.empty();
    });
    m_pAnimObjRenderingSystem->kdTree().setIntersectsCollectionFunc([this](const Ray&, const AnimatedModelRenderingSystem::ModelRenderingKdTreeNodeContainer& container) {
        this->m_vEntBuffer.insert(this->m_vEntBuffer.end(), container.m_vObjects.begin(), container.m_vObjects.end());
        return !container.m_vObjects.empty();
    });
    */
}

}

