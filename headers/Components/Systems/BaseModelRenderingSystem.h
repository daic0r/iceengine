#pragma once

#include <Entities/EntityManager.h>
#include <System/SystemServices.h>
#include <Components/TransformComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/MeshComponent.h>
#include <Components/SkeletonComponent.h>
#include <Components/Systems/CameraControllerSystem.h>
#include <Renderer/RenderEnvironment.h>
#include <System/Model.h>
#include <System/ModelInstance.h>
#include <Interfaces/IGraphicsSystem.h>
#include <Interfaces/IModelRenderer.h>
#include <System/KdTree.h>
#include <algorithm>
#include <ranges>
#include <Utils/ScopedTimeMeasurement.h>
#include <chrono>
#include <future>
#include <System/thread_pool.h>
#include <System/static_task.h>
#include <System/Octree.h>
#include <Components/Systems/TerrainSystem.h>
#include <Components/Systems/SceneGraphSystem.h>

namespace Ice {

struct MeshComponent;
class IModelRenderer;

class ABaseModelRenderingSystem {
	friend class SceneGraphSystem;

	static inline constexpr auto FRUSTUM_REFRESH_INTERVAL = 30;
protected:
	std::unordered_map<Model, std::vector<ModelInstance*>> m_vInstances;
	std::vector<Entity> m_vFrustumEnts;
	IModelRenderer* m_pRenderer{ nullptr };
	IModelRenderer* m_pShadowRenderer{ nullptr };
	IGraphicsSystem* m_pGraphicsSystem{};
	TerrainSystem* m_pTerrainSystem{};
	SceneGraphSystem* m_pSceneGraphSystem{};

	int m_nFramesSinceNoKdRefresh{}, m_nFramesSinceFrustumRefresh{};

	virtual bool isEntityEligibleForRendering(Entity e) const = 0;
	virtual bool isEntityEligibleForFrustumCulling(Entity e) const = 0;

	void onSystemsInitialized() noexcept;
	void render(const RenderEnvironment& env) noexcept;
	void willRemoveComponent(Entity e) noexcept;

public:
	const auto& entitiesInFrustum() const noexcept { return m_vFrustumEnts; }
};

template<typename ModelStructType, typename ModelInstanceType>
class BaseModelRenderingSystem : public ABaseModelRenderingSystem {
	static inline constexpr auto TREE_REFRESH_INTERVAL = 20;

public:
protected:
	std::unordered_map<Entity, std::pair<ModelStructType, ModelInstanceType>> m_vEntity2ModelStruct;

	virtual ModelStructType makeModelStruct(Entity) const = 0;

	static constexpr RenderSystem renderSystem() noexcept {
		if constexpr(std::is_same_v<ModelStructType, Model>)
			return RenderSystem::STATIC;
		else
			return RenderSystem::ANIMATED;
	}
	
	void onEntityAdded(Entity e) noexcept {
		auto& transf = entityManager.getSharedComponentOr<TransformComponent>(e);
		auto [iter, _] = m_vEntity2ModelStruct.emplace(e, std::make_pair(makeModelStruct(e), ModelInstanceType{}));
		//std::ranges::sort(m_vEntity2ModelStruct, [e](const auto& a, const auto& b) { return a.first < b.first; });
		iter->second.second.pTransform = &transf;
		
		if (std::ranges::none_of(m_vInstances, [iter](const auto& kvp) { return kvp.first.pMesh == iter->second.first.pMesh; })) {
			m_vInstances.emplace(iter->second.first, std::vector<ModelInstance*>{});
		}

		const AABB boxLocal{ iter->second.first.pMesh->extents() };
		const auto boxWorld = boxLocal.transform(iter->second.second.pTransform->m_transform);
		m_pSceneGraphSystem->tree().emplace(boxWorld, SceneGraphSystem::TreeEmplaceValue{ e, renderSystem(), iter->second.first, &iter->second.second });

		if (m_pShadowRenderer == nullptr)
			m_pShadowRenderer = systemServices.getShadowMapRenderer();

	}

	void willRemoveComponent(Entity e) noexcept {
		ABaseModelRenderingSystem::willRemoveComponent(e);
		{
			const auto iter = std::ranges::find_if(m_vEntity2ModelStruct, [e](const auto& kvp) { return e == kvp.first; });
			m_vEntity2ModelStruct.erase(iter);
		}
	}

	void render(const RenderEnvironment& env, const static_task<void(Entity, ModelInstanceType&)>& extendedUpdateInstanceFunc) noexcept {
		if (env.bMainRenderPass) {
			ABaseModelRenderingSystem::render(env);

			if (extendedUpdateInstanceFunc) {
				for (auto e : m_vFrustumEnts) {
					extendedUpdateInstanceFunc(e, m_vEntity2ModelStruct[e].second);
				}
			}
	
			if (m_pShadowRenderer)
				m_pShadowRenderer->render(env, m_vInstances);
		}
	}
};

}