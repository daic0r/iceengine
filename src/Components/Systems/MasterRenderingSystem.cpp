//
//  MasterRenderingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 05.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/MasterRenderingSystem.h>
#include <System/SystemServices.h>
#include <Interfaces/IMasterRenderer.h>
#include <Entities/EntityManager.h>
#include <Interfaces/IGraphicsSystem.h>
#include <Renderer/RenderEnvironment.h>
#include <Components/Systems/TerrainRenderingSystem.h>
#include <Components/Systems/CameraControllerSystem.h>
#include <GUI/WidgetManager.h>
#include <Interfaces/ITextManager.h>
#include <Interfaces/IWidgetRenderer.h>
#include <Renderer/Frustum.h>
#include <Components/Systems/DayNightCycleSystem.h>
#include <Interfaces/IModelRenderer.h>
#include <Interfaces/IShadowMapRenderer.h>
#include <Events/EventQueue.h>
#include <Events/WindowResizedEvent.h>

namespace Ice {

MasterRenderingSystem::MasterRenderingSystem() noexcept {
    //m_pRenderer = systemServices.getMasterRenderer();

    m_pTerrainRenderingSystem = std::make_unique<TerrainRenderingSystem>();
    entityManager.registerComponentSystem<false>(m_pTerrainRenderingSystem.get());
    m_pObjectRenderingSystem = std::make_unique<ObjectRenderingSystem>();
    entityManager.registerComponentSystem<true>(m_pObjectRenderingSystem.get());
    m_pAnimatedModelRenderingSystem = std::make_unique<AnimatedModelRenderingSystem>();
    entityManager.registerComponentSystem<true>(m_pAnimatedModelRenderingSystem.get());
    m_pSkyboxRenderingSystem = std::make_unique<SkyboxRenderingSystem>();
    entityManager.registerComponentSystem<true>(m_pSkyboxRenderingSystem.get());
    //m_pPathSegmentRenderingSystem = std::make_unique<PathSegmentRenderingSystem>();
    //entityManager.registerComponentSystem<false>(m_pPathSegmentRenderingSystem.get());
	//m_pParticleSystemSystem = std::make_unique<ParticleSystemSystem>();
	//entityManager.registerComponentSystem<true>(m_pParticleSystemSystem.get());
	m_pSunRenderingSystem = std::make_unique<SunRenderingSystem>();
	entityManager.registerComponentSystem<false>(m_pSunRenderingSystem.get());
	m_pWaterRenderingSystem = std::make_unique<WaterRenderingSystem>();
	entityManager.registerComponentSystem<false>(m_pWaterRenderingSystem.get());

    m_pPostProcessingPipeline = std::make_unique<PostProcessingPipeline>();

    m_pGraphicsSystem = systemServices.getGraphicsSystem();
    m_pWidgetRenderer = systemServices.getWidgetRenderer();
    m_pEventQueue = systemServices.getEventQueue();
}

bool MasterRenderingSystem::update(float fDeltaTime) {

	CameraComponent* pCam{};
	for (auto e : entities(entityManager.currentScene())) {
		CameraComponent *pThisComponent{};
		if ((pThisComponent = &entityManager.getComponent<CameraComponent>(e))->m_bPrimary) {
			pCam = pThisComponent;
			break;
		}
	}

    bool bDone{};
    while (auto pEvent = m_pEventQueue->peekInternalEvent()) {
        switch (pEvent->id()) {
            case EventId::WINDOW_RESIZED_EVENT:
            {
                const auto args = std::any_cast<WindowResizedEventArgs>(pEvent->args());
                m_pPostProcessingPipeline->resize(args.width(), args.height());
            }
                bDone = true;
                break;
            default:
                break;
        }
        if (bDone)
            break;
    }

	//const Camera& cam, float fDistNear, float fDistFar, float fovV, float fAspectRatio
	//Frustum f{ cam.m_camera, m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distFarPlane(), glm::radians(m_pGraphicsSystem->fov()), m_pGraphicsSystem->aspectRatio() };
    RenderEnvironment env;
    env.pCamera = &pCam->m_camera;
    env.fWaterLevel = std::nullopt;
    env.frustum = Frustum{ pCam->m_camera, m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distFarPlane(), glm::radians(m_pGraphicsSystem->fov()), m_pGraphicsSystem->aspectRatio() };
    env.viewMatrix = pCam->m_camera.matrix();
    env.projectionMatrix = m_pGraphicsSystem->projectionMatrix();
    env.fAspectRatio = m_pGraphicsSystem->aspectRatio();
    env.bWireframe = m_pGraphicsSystem->wireframe();
    env.bMainRenderPass = true;

	if (m_pDayNightSystem == nullptr)
		m_pDayNightSystem = entityManager.getSystem<DayNightCycleSystem, true>();

	env.pSun = std::addressof(entityManager.getComponent<SunComponent>(*m_pDayNightSystem->entities(entityManager.currentScene()).begin()).m_light);

    m_pPostProcessingPipeline->originalCanvas()->bind();
    m_pGraphicsSystem->beginRender();
    //m_pSkyboxRenderingSystem->render(env);
	dynamic_cast<IShadowMapRenderer*>(systemServices.getShadowMapRenderer())->clear();
    m_pObjectRenderingSystem->render(env);
    m_pAnimatedModelRenderingSystem->render(env);
    m_pTerrainRenderingSystem->render(env);
    m_pWaterRenderingSystem->render(env);
	m_pSunRenderingSystem->render(env);
    //m_pPathSegmentRenderingSystem->render(env);
	//m_pParticleSystemSystem->render(env);
    m_pPostProcessingPipeline->run();
	//dynamic_cast<IShadowMapRenderer*>(systemServices.getShadowMapRenderer())->renderShadowDepthTexture();
	//m_pObjectRenderingSystem->m_pRenderer->renderShadowDepthTexture();
    m_pWidgetRenderer->render(env, systemServices.getWidgetManager().rootLevelWidgets());
//    systemServices.getTextManager()->render();
    m_pGraphicsSystem->endRender();
//    m_pRenderer->render(fDeltaTime, &cam.m_camera);
    
    return true;
}

void MasterRenderingSystem::onSystemsInitialized() noexcept {
	m_pCameraControllerSystem = entityManager.getSystem<CameraControllerSystem, true>();

    m_pWaterRenderingSystem->setOriginalCanvas(m_pPostProcessingPipeline->originalCanvas());
}

IPostProcessingEffect* MasterRenderingSystem::addPostProcessingEffect(PostProcessingEffect effect)
{
    auto pEffect = PostProcessingEffectFactory::create(effect);
    auto pRet = pEffect.get();
    m_pPostProcessingPipeline->add(m_nEffectOrder++, std::move(pEffect));
    return pRet;
}


}
