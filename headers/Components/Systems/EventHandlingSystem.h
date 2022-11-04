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

class SceneGraphSystem;
class Ray;
struct CameraComponent;

class EventHandlingSystem : public EntityComponentSystem<InputReceiverComponent> {
	SceneGraphSystem* m_pSceneGraphSystem{};
	std::vector<Entity> m_vEntBuffer;
	Entity m_cameraEnt{};
public:
    
	EventHandlingSystem();
    bool update(float fDeltaTime) override;
	void onSystemsInitialized() noexcept override;

	bool isModelInstanceUnderMouse(Entity, const Ray& mouseRay, const CameraComponent&) const noexcept;
	void setCameraEnt(Entity e) noexcept { m_cameraEnt = e; }
	Entity cameraEnt() const noexcept { return m_cameraEnt; }

};

}

#endif /* EventHandlingSystem_h */