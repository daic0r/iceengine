//
//  SystemServices.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SystemServices_h
#define SystemServices_h

#include <IceEngine.h>
#include <memory>

namespace Ice {

#define systemServices Ice::SystemServices::instance()

class IParticleRenderer;
class IWaterRenderer;
class IModelRenderer;
//class IShaderManager;
class ITextureManager;
class IGraphicsSystem;
class ITextManager;
class ITerrainRenderer;
class IEventQueue;
class EntityManager;
class ISkyboxRenderer;
class IAnimatedModelRenderer;
class IPathSegmentRenderer;
class ITextRenderer;
class IWidgetRenderer;
class IComponentManager;
class ISunRenderer;
class IModelManager;
class Config;
namespace GUI {
    class WidgetManager;
    class ITextManager;
}

#define shaderManager Ice::SystemServices::instance().getShaderManager()
#define textureManager Ice::SystemServices::instance().getTextureManager()
#define masterRenderer Ice::SystemServices::instance().getMasterRenderer()
#define textManager Ice::SystemServices::instance().getTextManager()
#define eventQueue Ice::SystemServices::instance().getEventQueue()
#define entityManager Ice::SystemServices::instance().getEntityManager()
#define componentManager Ice::SystemServices::instance().getComponentManager()

class ICEENGINE_API SystemServices {
    std::unique_ptr<IParticleRenderer> m_pParticleRenderer;
    std::unique_ptr<IWaterRenderer> m_pWaterRenderer;
    std::unique_ptr<IModelRenderer> m_pModelRenderer;
    //std::unique_ptr<IShaderManager> m_pShaderManager;
    std::unique_ptr<ITextureManager> m_pTextureManager;
    std::unique_ptr<IGraphicsSystem> m_pGraphicsSystem;
    std::unique_ptr<GUI::ITextManager> m_pTextManager;
    std::unique_ptr<ITerrainRenderer> m_pTerrainRenderer;
    std::unique_ptr<ISkyboxRenderer> m_pSkyboxRenderer;
    std::unique_ptr<IModelRenderer> m_pAniModelRenderer;
    std::unique_ptr<IPathSegmentRenderer> m_pPathSegmentRenderer;
    std::unique_ptr<ITextRenderer> m_pTextRenderer;
    std::unique_ptr<IWidgetRenderer> m_pWidgetRenderer;
    std::unique_ptr<IEventQueue> m_pEventQueue;
	std::unique_ptr<IComponentManager> m_pComponentManager;
	std::unique_ptr<ISunRenderer> m_pSunRenderer;
	std::unique_ptr<IModelManager> m_pModelManager;
	std::unique_ptr<IModelRenderer> m_pShadowMapRenderer;
    EntityManager& m_entityManager;
    GUI::WidgetManager& m_widgetManager;
    
    SystemServices();
    ~SystemServices();
    void shutDown();
    
public:
    static SystemServices& instance();
    
    IParticleRenderer* getParticleRenderer() noexcept;
    IWaterRenderer* getWaterRenderer() noexcept;
    IModelRenderer* getModelRenderer() noexcept;
    //IShaderManager* getShaderManager() noexcept;
    ITextureManager* getTextureManager() noexcept;
    IGraphicsSystem* getGraphicsSystem() noexcept;
    GUI::ITextManager* getTextManager() noexcept;
    ITerrainRenderer* getTerrainRenderer() noexcept;
    ISkyboxRenderer* getSkyboxRenderer() noexcept;
    IModelRenderer* getAnimatedModelRenderer() noexcept;
    IPathSegmentRenderer* getPathSegmentRenderer() noexcept;
    ITextRenderer* getTextRenderer() noexcept;
    IWidgetRenderer* getWidgetRenderer() noexcept;
	IComponentManager* getComponentManager() noexcept;
    IEventQueue* getEventQueue() noexcept;
	ISunRenderer* getSunRenderer() noexcept;
	IModelManager* getModelManager() noexcept;
	IModelRenderer* getShadowMapRenderer() noexcept;
    EntityManager& getEntityManager() noexcept { return m_entityManager; }
    GUI::WidgetManager& getWidgetManager() noexcept { return m_widgetManager; }
    Config* getConfig() const noexcept;
    void setConfig(const Config&) noexcept;

	template<typename T> void registerComponentManager() noexcept {
		m_pComponentManager = std::make_unique<T>();
	}
};

}

#endif /* SystemServices_h */
