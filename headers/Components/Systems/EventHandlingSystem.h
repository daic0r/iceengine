//
//  EventHandlingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef EventHandlingSystem_h
#define EventHandlingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/InputReceiverComponent.h>

namespace Ice {

class ObjectRenderingSystem;
class AnimatedModelRenderingSystem;
struct CameraComponent;

class EventHandlingSystem : public EntityComponentSystem<InputReceiverComponent> {
	ObjectRenderingSystem* m_pObjRenderingSystem{ nullptr };
	AnimatedModelRenderingSystem* m_pAnimObjRenderingSystem{ nullptr };
	std::vector<Entity> m_vEntBuffer;
	Entity m_cameraEnt{};
public:
    
	EventHandlingSystem();
    bool update(float fDeltaTime) override;
	void onSystemsInitialized() noexcept override;

	bool isModelInstanceUnderMouse(Entity, const CameraComponent&) const noexcept;
	void setCameraEnt(Entity e) noexcept { m_cameraEnt = e; }
	Entity cameraEnt() const noexcept { return m_cameraEnt; }

};

}

#endif /* EventHandlingSystem_h */
