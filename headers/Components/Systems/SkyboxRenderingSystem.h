//
//  SkyboxRenderingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SkyboxRenderingSystem_h
#define SkyboxRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/SkyboxComponent.h>

namespace Ice {

class ISkyboxRenderer;
struct RenderEnvironment;

class SkyboxRenderingSystem : public EntityComponentSystem<SkyboxComponent> {
    
    ISkyboxRenderer* m_pRenderer{ nullptr };
	float m_fRotation{ 0.0f };
    
public:
    SkyboxRenderingSystem() noexcept;
    void render(const RenderEnvironment&) noexcept;
	bool update(float fDeltaTime) noexcept;
};

}

#endif /* SkyboxRenderingSystem_h */
