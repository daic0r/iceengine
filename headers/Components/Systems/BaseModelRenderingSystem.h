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

namespace Ice {

struct MeshComponent;
class IModelRenderer;

template<typename ModelStructType, typename ModelInstanceType>
class BaseModelRenderingSystem {
	static inline constexpr auto KDTREE_REFRESH_INTERVAL = 10;
	static inline constexpr auto FRUSTUM_REFRESH_INTERVAL = 30;
protected:
	//std::vector<std::pair<Model, std::vector<ModelInstance*>>> m_vInstances;
	std::unordered_map<Model, std::vector<ModelInstance*>> m_vInstances;
	//std::vector<std::pair<Entity, std::pair<ModelStructType, ModelInstanceType>>> m_vEntity2ModelStruct;
	std::unordered_map<Entity, std::pair<ModelStructType, ModelInstanceType>> m_vEntity2ModelStruct;
	std::vector<Entity> m_vFrustumEnts;
	CameraControllerSystem* m_pCameraControllerSystem{ nullptr };
	IModelRenderer* m_pRenderer{ nullptr };
	IModelRenderer* m_pShadowRenderer{ nullptr };
	IGraphicsSystem* m_pGraphicsSystem{};
	KdTree<Entity> m_kdTree{};
	std::vector<Entity> m_vVisibleEnts{};
	int m_nFramesSinceNoKdRefresh{}, m_nFramesSinceFrustumRefresh{};
	std::vector<glm::vec3> m_vKdTreeVertices;

	virtual ModelStructType makeModelStruct(Entity) const = 0;
	virtual bool isEntityEligibleForRendering(Entity e) const = 0;
	virtual bool isEntityEligibleForFrustumCulling(Entity e) const = 0;
	
	void onSystemsInitialized() noexcept {
		m_pCameraControllerSystem = entityManager.getSystem<CameraControllerSystem, true>();
		m_pGraphicsSystem = systemServices.getGraphicsSystem();
//		m_pShadowRenderer = systemServices.getShadowMapRenderer();
	}

	void onEntityAdded(Entity e) noexcept {
		auto& transf = entityManager.getSharedComponentOr<TransformComponent>(e);
		auto [iter, _] = m_vEntity2ModelStruct.emplace(e, std::make_pair(makeModelStruct(e), ModelInstanceType{}));
		//std::ranges::sort(m_vEntity2ModelStruct, [e](const auto& a, const auto& b) { return a.first < b.first; });
		iter->second.second.pTransform = &transf;
		
		if (std::ranges::none_of(m_vInstances, [iter](const auto& kvp) { return kvp.first.pMesh == iter->second.first.pMesh; })) {
			m_vInstances.emplace(iter->second.first, std::vector<ModelInstance*>{});
		}
		if (m_pShadowRenderer == nullptr)
			m_pShadowRenderer = systemServices.getShadowMapRenderer();

	}

	void willRemoveComponent(Entity e) noexcept {
		{
			const auto iter = std::ranges::find_if(m_vEntity2ModelStruct, [e](const auto& kvp) { return e == kvp.first; });
			m_vEntity2ModelStruct.erase(iter);
		}
		{
			auto iter = std::ranges::find(m_vFrustumEnts, e);
			if (iter != m_vFrustumEnts.end())
				m_vFrustumEnts.erase(iter);
		}
	}

	bool update(float fDeltaTime, const std::set<Entity>& ents) noexcept {
		if (m_nFramesSinceNoKdRefresh % KDTREE_REFRESH_INTERVAL == 0) {
			buildKdTree();	
			m_nFramesSinceNoKdRefresh %= KDTREE_REFRESH_INTERVAL;
			m_nFramesSinceNoKdRefresh = 1;
			std::cout << "Bauta\n";
		}
		return true;
	}

	void render(const RenderEnvironment& env, const std::function<void(Entity, ModelInstanceType&)>& extendedUpdateInstanceFunc) noexcept {
		/*
		if (m_nFramesSinceFrustumRefresh % FRUSTUM_REFRESH_INTERVAL == 0) {
			const auto& ents = entitiesInFrustum();
		
			// Clear all the contained vectors but don't destruct them to prevent
			// having to reallocate memory
			for (auto& kvp : m_vInstances) {
				kvp.second.clear();
			}
			// This loop makes framerate drop from 500 to 200 fps
			// #OPTIMIZE
			for (auto e : ents) {
				if (!isEntityEligibleForRendering(e))
					continue;

				auto& [model, inst] = m_vEntity2ModelStruct.find(e)->second;
				extendedUpdateInstanceFunc(e, inst);
				auto iter2 = std::ranges::find_if(m_vInstances, [&model](const auto& kvp) { return kvp.first.pMesh == model.pMesh; });
				iter2->second.push_back(&inst);
			}
			m_nFramesSinceFrustumRefresh = 1;
		} else {
			++m_nFramesSinceFrustumRefresh;
		}
		*/
		if (env.bMainRenderPass) {
			if (m_nFramesSinceFrustumRefresh % FRUSTUM_REFRESH_INTERVAL == 0) {
				m_vFrustumEnts.clear();
				// PROFILING: SUPER FAST
				// #DONOTOPTIMIZE
				for (auto& kvp : m_vInstances) {
					kvp.second.clear();
				}
				m_kdTree.getVisibleObjects(&env.frustum, m_vFrustumEnts, m_vInstances);
				m_nFramesSinceFrustumRefresh = 1;
			} else {
				++m_nFramesSinceFrustumRefresh;
			}
	
			if (m_pShadowRenderer)
				m_pShadowRenderer->render(env, m_vInstances);
		}
	}

public:
	const auto& kdTree() const noexcept { return m_kdTree; }

	const auto& entitiesInFrustum() const noexcept { return m_vFrustumEnts; }
	void buildKdTree() {
		m_vKdTreeVertices.clear();

		// Measured: WAY faster >without< multithreading the loop below
		for (const auto& [e, modelInstPair] : m_vEntity2ModelStruct) {
			AABB boxLocal{ modelInstPair.first.pMesh->extents() };

			const auto boxWorld = boxLocal.transform(modelInstPair.second.pTransform->m_transform);
			//m_vKdTreeVertices.push_back(boxWorld.minVertex());
			//m_vKdTreeVertices.push_back(boxWorld.maxVertex());

			const auto worldCorners = boxWorld.cornerVertices();
			for (const auto& corner : worldCorners) {
				m_vKdTreeVertices.emplace_back(corner[0], corner[1], corner[2]);
			}
/*				USE ALL CORNERS?
*/
		}

		{
			/*
			ScopedTimeMeasurement m([](std::chrono::nanoseconds dur) {
				std::cout << "Took " << dur.count() << "ns\n";
			});
			std::cout << "Constructing kd-Tree from " << m_vKdTreeVertices.size() << " points\n";
			*/
			m_kdTree.construct(std::move(m_vKdTreeVertices));
		}
		for (auto& [ent, modelInstPair] : m_vEntity2ModelStruct) {
			m_kdTree.emplace(glm::vec3{ modelInstPair.second.pTransform->m_transform * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f } }, ent, modelInstPair.first, &modelInstPair.second);
		}
	}
};

}