//
//  MasterRenderingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 05.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MasterRenderingSystem_h
#define MasterRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/CameraComponent.h>
#include <Components/Systems/TerrainRenderingSystem.h>
#include <Components/Systems/ObjectRenderingSystem.h>
#include <Components/Systems/SkyboxRenderingSystem.h>
#include <Components/Systems/AnimatedModelRenderingSystem.h>
#include <Components/Systems/PathSegmentRenderingSystem.h>
#include <Components/Systems/ParticleSystemSystem.h>
#include <Components/Systems/SunRenderingSystem.h>
#include <Components/Systems/WaterRenderingSystem.h>
#include <Renderer/PostProcessing/PostProcessingPipeline.h>
#include <Renderer/PostProcessing/PostProcessingEffectFactory.h>

namespace Ice {

class IGraphicsSystem;
class IWidgetRenderer;
class DayNightCycleSystem;
class IEventQueue;

class MasterRenderingSystem : public EntityComponentSystem<CameraComponent> {
	friend class Engine;

    //IMasterRenderer *m_pRenderer;
    std::unique_ptr<TerrainRenderingSystem> m_pTerrainRenderingSystem;
    std::unique_ptr<ObjectRenderingSystem> m_pObjectRenderingSystem;
    std::unique_ptr<SkyboxRenderingSystem> m_pSkyboxRenderingSystem;
    std::unique_ptr<AnimatedModelRenderingSystem> m_pAnimatedModelRenderingSystem;
    std::unique_ptr<PathSegmentRenderingSystem> m_pPathSegmentRenderingSystem;
	std::unique_ptr<ParticleSystemSystem> m_pParticleSystemSystem;
	std::unique_ptr<SunRenderingSystem> m_pSunRenderingSystem;
    std::unique_ptr<WaterRenderingSystem> m_pWaterRenderingSystem;
    std::unique_ptr<PostProcessingPipeline> m_pPostProcessingPipeline;
    IGraphicsSystem *m_pGraphicsSystem{ nullptr };
    IWidgetRenderer *m_pWidgetRenderer{ nullptr };
    IEventQueue *m_pEventQueue{};
	CameraControllerSystem* m_pCameraControllerSystem{ nullptr };
	DayNightCycleSystem* m_pDayNightSystem{ nullptr };
    std::size_t m_nEffectOrder{ 0 };
    
public:
    
    MasterRenderingSystem() noexcept;
    bool update(float fDeltaTime) override;
	void onSystemsInitialized() noexcept override;

    IPostProcessingEffect* addPostProcessingEffect(PostProcessingEffect effect);
};

}

#endif /* MasterRenderingSystem_h */
