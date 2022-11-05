#include <Components/Systems/BaseModelRenderingSystem.h>

namespace Ice
{
    void ABaseModelRenderingSystem::onSystemsInitialized() noexcept {
		m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
		m_pGraphicsSystem = systemServices.getGraphicsSystem();
		m_pSceneGraphSystem = entityManager.getSystem<SceneGraphSystem, true>();
    }

	void ABaseModelRenderingSystem::render(const RenderEnvironment& env) noexcept {
        if (m_nFramesSinceFrustumRefresh % FRUSTUM_REFRESH_INTERVAL == 0) {
            m_vFrustumEnts.clear();
            // PROFILING: SUPER FAST
            // #DONOTOPTIMIZE
            for (auto& kvp : m_vInstances) {
                kvp.second.clear();
            }
            m_pSceneGraphSystem->tree().getVisibleObjects(env.frustum); //, m_vFrustumEnts, m_vInstances);
            m_nFramesSinceFrustumRefresh = 1;
        } else {
            ++m_nFramesSinceFrustumRefresh;
        }
    }


	void ABaseModelRenderingSystem::willRemoveComponent(Entity e) noexcept {
        auto iter = std::ranges::find(m_vFrustumEnts, e);
        if (iter != m_vFrustumEnts.end())
            m_vFrustumEnts.erase(iter);
    }

} // namespace Ice
