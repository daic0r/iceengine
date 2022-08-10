//
//  CameraControllerSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef CameraControllerSystem_h
#define CameraControllerSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/TerrainComponent.h>
#include <Components/CameraComponent.h>
#include <Renderer/Frustum.h>

namespace Ice {

class IGraphicsSystem;
class TerrainSystem;

class CameraControllerSystem : public EntityComponentSystem<CameraComponent, TerrainComponent> {
  
    int nLastX{}, nLastY{};
    bool bFirstMouse{ true };
	Frustum m_frustum;
	IGraphicsSystem* m_pGraphicsSystem{ nullptr };
	TerrainSystem* m_pTerrainSystem{};

public:
    
	CameraControllerSystem();
    bool update(float fDeltaTime) override;
	// Frustum for which camera? For later -> for now just one frustum for the first camera
	const Frustum& frustum(Entity e = INVALID_ENTITY_ID) const noexcept { return m_frustum; }
	Frustum transformedFrustum(const glm::mat4& m, Entity e = INVALID_ENTITY_ID) const noexcept;
	int executionOrder() const noexcept override { return 0; }
};

}

#endif /* CameraControllerSystem_h */
