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
protected:
	std::vector<std::pair<Model, std::vector<ModelInstance*>>> m_vInstances;
	std::vector<std::pair<Entity, std::pair<ModelStructType, ModelInstanceType>>> m_vEntity2ModelStruct;
	std::set<Entity> m_sFrustumEnts;
	CameraControllerSystem* m_pCameraControllerSystem{ nullptr };
	IModelRenderer* m_pRenderer{ nullptr };
	IModelRenderer* m_pShadowRenderer{ nullptr };
	IGraphicsSystem* m_pGraphicsSystem{};
	KdTree<Entity> m_kdTree{};
	std::vector<Entity> m_vVisibleEnts{};
	int m_nFramesSinceNoKdRefresh{};
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
		auto iter = m_vEntity2ModelStruct.emplace(m_vEntity2ModelStruct.end(), e, std::make_pair(makeModelStruct(e), ModelInstanceType{}));
		iter->second.second.pTransform = &transf;
		
		if (std::ranges::none_of(m_vInstances, [iter](const auto& kvp) { return kvp.first.pMesh == iter->second.first.pMesh; })) {
			m_vInstances.emplace_back(iter->second.first, std::vector<ModelInstance*>{});
		}
		if (m_pShadowRenderer == nullptr)
			m_pShadowRenderer = systemServices.getShadowMapRenderer();

	}

	void willRemoveComponent(Entity e) noexcept {
		const auto iter = std::ranges::find_if(m_vEntity2ModelStruct, [e](const auto& kvp) { return e == kvp.first; });
		m_vEntity2ModelStruct.erase(iter);
		m_sFrustumEnts.erase(e);
	}

	bool update(float fDeltaTime, const std::set<Entity>& ents) noexcept {
		if (m_nFramesSinceNoKdRefresh++ % KDTREE_REFRESH_INTERVAL == 0) {
			buildKdTree();	
			m_nFramesSinceNoKdRefresh %= KDTREE_REFRESH_INTERVAL;
		}
		m_sFrustumEnts.clear();
		m_vVisibleEnts.clear();
		const auto& frustum = m_pCameraControllerSystem->frustum();
		{
			/*ScopedTimeMeasurement m([](std::chrono::nanoseconds d) {
				std::cout << d.count() << " ns\n";	
			});*/
			m_kdTree.getVisibleObjects(&frustum, m_vVisibleEnts);
			//std::cout << "Have " << m_vVisibleEnts.size() << " elements\n";
		}
		//m_kdTree.getVisibleObjects(&frustum, m_vVisibleEnts);
		std::move(m_vVisibleEnts.begin(), m_vVisibleEnts.end(), std::inserter( m_sFrustumEnts, m_sFrustumEnts.end()));
	/*
		for (auto e : ents) {
			if (!isEntityEligibleForFrustumCulling(e))
				continue;

			auto& transf = entityManager.getSharedComponentOr<TransformComponent>(e);
			const auto& refComp = entityManager.getComponent<ModelReferenceComponent>(e);
			const auto& meshComp = entityManager.getComponent<MeshComponent>(refComp.m_entity);
			// Frustum culling before anything elee
			const auto minWorld = meshComp.extents().minPoint;
			const auto maxWorld = meshComp.extents().maxPoint;
			//minWorld = transf.m_transform * glm::vec4{ minWorld, 1.0f };
			//maxWorld = transf.m_transform * glm::vec4{ maxWorld, 1.0f };
			const auto frustum = m_pCameraControllerSystem->transformedFrustum(glm::inverse(transf.m_transform));
			//if (frustum.checkMinMaxBounds(minWorld, maxWorld, false) || frustum.checkMinMaxBounds(minWorld, maxWorld, true)) {
			if (frustum.intersects(minWorld, maxWorld, false) != FrustumAABBIntersectionType::NO_INTERSECTION) {
				m_sFrustumEnts.insert(e);
			}
		}
		std::cout << "Han " << m_sFrustumEnts.size() << " elements\n";
		*/
		return true;
	}

	void render(const RenderEnvironment& env, const std::function<void(Entity, ModelInstanceType&)>& extendedUpdateInstanceFunc) noexcept {
		const auto& ents = entitiesInFrustum();
    
		// Clear all the contained vectors but don't destruct them to prevent
		// having to reallocate memory
		for (auto& kvp : m_vInstances) {
			kvp.second.clear();
		}
		for (auto e : ents) {
			if (!isEntityEligibleForRendering(e))
				continue;

			auto& [model, inst] = std::ranges::find_if(m_vEntity2ModelStruct, [e](const auto& kvp) { return e == kvp.first; })->second;
			extendedUpdateInstanceFunc(e, inst);
			auto iter = std::ranges::find_if(m_vInstances, [&model](const auto& kvp) { return kvp.first.pMesh == model.pMesh; });
			iter->second.push_back(&inst);
		}
		if (m_pShadowRenderer)
			m_pShadowRenderer->render(env, m_vInstances);
	}

public:
	const auto& kdTree() const noexcept { return m_kdTree; }

	const std::set<Entity>& entitiesInFrustum() const noexcept { return m_sFrustumEnts; }
	void buildKdTree() {
		m_vKdTreeVertices.clear();

		// Measured: WAY faster >without< multithreading the loop below
		for (const auto& [e, modelInstPair] : m_vEntity2ModelStruct) {
			AABB boxLocal{ modelInstPair.first.pMesh->extents() };

			const auto boxWorld = boxLocal.transform(modelInstPair.second.pTransform->m_transform);
			m_vKdTreeVertices.push_back(boxWorld.minVertex());
			m_vKdTreeVertices.push_back(boxWorld.maxVertex());

/*				USE ALL CORNERS?
			const auto worldCorners = boxWorld.cornerVertices();
			for (const auto& corner : worldCorners) {
				vVertices.emplace_back(corner[0], corner[1], corner[2]);
			}
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
		for (const auto& [ent, modelInstPair] : m_vEntity2ModelStruct) {
			m_kdTree.emplace(glm::vec3{ modelInstPair.second.pTransform->m_transform * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f } }, ent);
		}
	}
};

}