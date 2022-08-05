//
//  SystemServices.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/SystemServices.h>
#ifdef RENDERER_OPEN_GL
#include <Renderer/OpenGL/ParticleRendererGL.h>
#include <Renderer/OpenGL/WaterRendererGL.h>
#include <Renderer/OpenGL/ModelRendererGL.h>
#include <Renderer/OpenGL/GraphicsSystemGL.h>
#include <Renderer/OpenGL/TerrainRendererGL.h>
#include <Renderer/OpenGL/GraphicsSystemGL.h>
#include <Renderer/OpenGL/SkyboxRendererGL.h>
#include <Renderer/OpenGL/AnimatedModelRendererGL.h>
#include <Renderer/OpenGL/PathSegmentRendererGL.h>
#include <Renderer/OpenGL/TextRendererGL.h>
#include <Renderer/OpenGL/WidgetRendererGL.h>
#include <Renderer/OpenGL/TextureManagerGL.h>
#include <Renderer/OpenGL/SunRendererGL.h>
#include <Renderer/OpenGL/ModelManagerGL.h>
#include <Renderer/OpenGL/ShadowMapRendererGL.h>
#endif

//#include <Renderer/ShaderManager.h>
//#include <Renderer/TextureManager.h>
#include <GUI/Fonts/TextManager.h>
#include <Entities/EntityManager.h>
#include <Events/EventQueue.h>
#include <Components/ComponentManager.h>
#include <GUI/WidgetManager.h>
#include <System/Config.h>

namespace Ice {

EntityManager _entityManager;
GUI::WidgetManager _widgetManager;
Config _config;

SystemServices::SystemServices() : m_entityManager{ _entityManager }, m_widgetManager { _widgetManager } {
}

SystemServices::~SystemServices() {
    shutDown();
}

void SystemServices::shutDown() {
    m_pParticleRenderer.reset();
    m_pWaterRenderer.reset();
    m_pModelRenderer.reset();
    m_pTerrainRenderer.reset();
    m_pSkyboxRenderer.reset();
    m_pAniModelRenderer.reset();
    m_pParticleRenderer.reset();
    m_pTextRenderer.reset();
    m_pWidgetRenderer.reset();
	m_pPathSegmentRenderer.reset();
	m_pModelManager.reset();
	m_pSunRenderer.reset();
	m_pShadowMapRenderer.reset();

    m_pTextManager.reset();
    m_pEventQueue.reset();

    m_pGraphicsSystem.reset();
}

SystemServices& SystemServices::instance() {
    static SystemServices _inst;
    return _inst;
}

IParticleRenderer* SystemServices::getParticleRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pParticleRenderer == nullptr) {
        m_pParticleRenderer = std::make_unique<ParticleRendererGL>();
    }
#endif
    return m_pParticleRenderer.get();
}

IWaterRenderer* SystemServices::getWaterRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pWaterRenderer == nullptr) {
        m_pWaterRenderer = std::make_unique<WaterRendererGL>();
    }
#endif
    return m_pWaterRenderer.get();
}

IModelRenderer* SystemServices::getModelRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pModelRenderer == nullptr) {
        m_pModelRenderer = std::make_unique<ModelRendererGL>();
    }
#endif
    return m_pModelRenderer.get();
}

//IShaderManager* SystemServices::getShaderManager() noexcept {
//    if (m_pShaderManager == nullptr) {
//        m_pShaderManager = std::make_unique<ShaderManager>();
//    }
//    return m_pShaderManager.get();
//}

ITextureManager* SystemServices::getTextureManager() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pTextureManager == nullptr) {
        m_pTextureManager = std::make_unique<TextureManagerGL>();
    }
#endif
    return m_pTextureManager.get();
}

GUI::ITextManager* SystemServices::getTextManager() noexcept {
    if (m_pTextManager == nullptr) {
        m_pTextManager = std::make_unique<GUI::TextManager>();
    }
    return m_pTextManager.get();
}

ITextRenderer* SystemServices::getTextRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pTextRenderer == nullptr) {
        m_pTextRenderer = std::make_unique<TextRendererGL>();
    }
#endif
    return m_pTextRenderer.get();
}

IGraphicsSystem* SystemServices::getGraphicsSystem() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pGraphicsSystem == nullptr) {
        m_pGraphicsSystem = std::make_unique<GraphicsSystemGL>();
    }
#endif
    return m_pGraphicsSystem.get();
}

ITerrainRenderer* SystemServices::getTerrainRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pTerrainRenderer == nullptr) {
        m_pTerrainRenderer = std::make_unique<TerrainRendererGL>();
    }
#endif
    return m_pTerrainRenderer.get();
}

ISkyboxRenderer* SystemServices::getSkyboxRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pSkyboxRenderer == nullptr) {
        m_pSkyboxRenderer = std::make_unique<SkyboxRendererGL>();
    }
#endif
    return m_pSkyboxRenderer.get();
}

IModelRenderer* SystemServices::getAnimatedModelRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pAniModelRenderer == nullptr) {
        m_pAniModelRenderer = std::make_unique<AnimatedModelRendererGL>();
    }
#endif
    return m_pAniModelRenderer.get();
}

IPathSegmentRenderer* SystemServices::getPathSegmentRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pPathSegmentRenderer == nullptr) {
        m_pPathSegmentRenderer = std::make_unique<PathSegmentRendererGL>();
    }
#endif
    return m_pPathSegmentRenderer.get();
}

IWidgetRenderer* SystemServices::getWidgetRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pWidgetRenderer == nullptr) {
        m_pWidgetRenderer = std::make_unique<WidgetRendererGL>();
    }
#endif
    return m_pWidgetRenderer.get();
}

ISunRenderer* SystemServices::getSunRenderer() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pSunRenderer == nullptr) {
        m_pSunRenderer = std::make_unique<SunRendererGL>();
    }
#endif
    return m_pSunRenderer.get();
}

IModelManager* SystemServices::getModelManager() noexcept {
#ifdef RENDERER_OPEN_GL
    if (m_pModelManager == nullptr) {
        m_pModelManager = std::make_unique<ModelManagerGL>();
    }
#endif
    return m_pModelManager.get();
}

IModelRenderer* SystemServices::getShadowMapRenderer() noexcept
{
#ifdef RENDERER_OPEN_GL
    if (m_pShadowMapRenderer == nullptr) {
        m_pShadowMapRenderer = std::make_unique<ShadowMapRendererGL>();
    }
#endif
    return m_pShadowMapRenderer.get();
}

IEventQueue* SystemServices::getEventQueue() noexcept {
    if (m_pEventQueue == nullptr) {
        m_pEventQueue = std::make_unique<EventQueue>();
    }
    return m_pEventQueue.get();
}

IComponentManager* SystemServices::getComponentManager() noexcept {
	if (m_pComponentManager == nullptr) {
		m_pComponentManager = std::make_unique<ComponentManager>();
	}
	return m_pComponentManager.get();
}

Config* SystemServices::getConfig() const noexcept {
    return &_config;
}

void SystemServices::setConfig(const Config& c) noexcept {
    _config = c;
}

}
