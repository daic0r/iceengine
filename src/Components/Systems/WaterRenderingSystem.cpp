#include <Components/Systems/WaterRenderingSystem.h>
#include <System/SystemServices.h>
#include <Interfaces/IWaterRenderer.h>
#include <Renderer/RenderEnvironment.h>
#include <Entities/EntityManager.h>

namespace Ice
{
    void WaterRenderingSystem::onSystemsInitialized() noexcept {
        m_pRenderer = systemServices.getWaterRenderer();
    }

    void WaterRenderingSystem::render(const RenderEnvironment& env) noexcept {
        m_vTiles.clear();
        for (auto e : entities(entityManager.currentScene())) {
            auto& comp = entityManager.getComponent<WaterTileComponent>(e);
            m_vTiles.push_back(&comp.m_tile);
        }
        m_pRenderer->render(env, m_vTiles);
    }
} // namespace Ice
