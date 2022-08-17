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
protected:
	std::unordered_map<const Model*, std::vector<ModelInstance*>> m_mInstances;
	std::map<MeshComponent*, ModelStructType> m_mModels;
	std::map<Entity, ModelInstanceType> m_mInstanceBuffer;
	std::set<Entity> m_sFrustumEnts;
	CameraControllerSystem* m_pCameraControllerSystem{ nullptr };
	IModelRenderer* m_pRenderer{ nullptr };
	IModelRenderer* m_pShadowRenderer{ nullptr };
	IGraphicsSystem* m_pGraphicsSystem{};
	KdTree<Entity> m_kdTree{};
	std::vector<Entity> m_vVisibleEnts{};
	bool m_bConstruction{ true };

	virtual ModelStructType makeModelStruct(Entity) const = 0;
	virtual bool isEntityEligibleForRendering(Entity e) const = 0;
	virtual bool isEntityEligibleForFrustumCulling(Entity e) const = 0;
	
	void onSystemsInitialized() noexcept {
		m_pCameraControllerSystem = entityManager.getSystem<CameraControllerSystem, true>();
		m_pGraphicsSystem = systemServices.getGraphicsSystem();
//		m_pShadowRenderer = systemServices.getShadowMapRenderer();
	}

	void onEntityAdded(Entity e) noexcept {
		auto& ref = entityManager.getComponent<ModelReferenceComponent>(e);
		auto& mesh = entityManager.getComponent<MeshComponent>(ref.m_entity);
		auto& transf = entityManager.getSharedComponentOr<TransformComponent>(e);

		ModelStructType* pModel{ nullptr };
		auto modelIter = m_mModels.find(&mesh);
		if (modelIter == m_mModels.end()) {
			bool bSucc;
			std::tie(modelIter, bSucc) = m_mModels.emplace(&mesh, makeModelStruct(e));
			auto instanceIter = m_mInstanceBuffer.emplace(e, ModelInstanceType{}).first;
			instanceIter->second.pTransform = &transf; 
			auto [insertIter, succ] = m_mInstances.emplace(&modelIter->second, std::vector<ModelInstance*>{});
			m_mInstances.at(&modelIter->second).push_back(&instanceIter->second);
		}
		pModel = std::addressof(modelIter->second);
		auto instanceIter = m_mInstanceBuffer.find(e);
		if (instanceIter == m_mInstanceBuffer.end()) {
			instanceIter = m_mInstanceBuffer.emplace(e, ModelInstanceType{}).first;
			instanceIter->second.pTransform = &transf; 
			m_mInstances.at(&modelIter->second).push_back(&instanceIter->second);
		}
		//m_pRenderer->registerModel(pModel);
		//m_pCameraControllerSystem = entityManager.getSystem<CameraControllerSystem, true>();
		if (m_pShadowRenderer == nullptr)
			m_pShadowRenderer = systemServices.getShadowMapRenderer();

	}

	void willRemoveComponent(Entity e) noexcept {
		if (m_mInstanceBuffer.contains(e)) {
			const auto& ref = entityManager.getComponent<ModelReferenceComponent>(e);
			auto& mesh = entityManager.getComponent<MeshComponent>(ref.m_entity);
			auto pModel = &m_mModels.at(&mesh);
			auto& vInstPtrs = m_mInstances.at(pModel);
			auto instanceIter = m_mInstanceBuffer.find(e);
			auto toDelIter = std::find(vInstPtrs.begin(), vInstPtrs.end(), &instanceIter->second);
			vInstPtrs.erase(toDelIter);
			m_mInstanceBuffer.erase(instanceIter);
			m_sFrustumEnts.erase(e);
		}
	}

	bool update(float fDeltaTime, const std::set<Entity>& ents) noexcept {
		m_sFrustumEnts.clear();
		m_vVisibleEnts.clear();
		const auto& frustum = m_pCameraControllerSystem->frustum();
		if constexpr (std::is_same_v<ModelStructType, Model>) {
			{
				ScopedTimeMeasurement m([](std::chrono::nanoseconds d) {
					std::cout << d.count() << " ns\n";	
				});
				m_kdTree.getVisibleObjects(&frustum, m_vVisibleEnts);
			}
			std::cout << "Have " << m_vVisibleEnts.size() << " elements\n";
			std::move(m_vVisibleEnts.begin(), m_vVisibleEnts.end(), std::inserter( m_sFrustumEnts, m_sFrustumEnts.end()));
		}
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
		for (auto& kvp : m_mInstances) {
			kvp.second.clear();
		}
		for (auto e : ents) {
			if (!isEntityEligibleForRendering(e))
				continue;
        
			auto& transf = entityManager.getSharedComponentOr<TransformComponent>(e);

			auto instanceIter = m_mInstanceBuffer.find(e);
			instanceIter->second.pTransform = &transf;
			extendedUpdateInstanceFunc(e, instanceIter->second);

			auto& ref = entityManager.getComponent<ModelReferenceComponent>(e);
			auto& mesh = entityManager.getComponent<MeshComponent>(ref.m_entity);
			auto& model = m_mModels.at(&mesh);
			m_mInstances[&model].push_back(&instanceIter->second);
		}
		if (m_pShadowRenderer)
			m_pShadowRenderer->render(env, m_mInstances);
	}

public:
	const std::set<Entity>& entitiesInFrustum() const noexcept { return m_sFrustumEnts; }
	bool inConstruction() const noexcept { return m_bConstruction; }
	void setInConstruction(bool b) noexcept { m_bConstruction = b; }
	void finishConstruction() {
		std::vector<float> vVertices;
		for (const auto& [pModel, vInst] : m_mInstances) {
			AABB boxLocal{ pModel->pMesh->extents() };

			for (auto pModelInstance : vInst) {
				AABB boxWorld{};
				const auto corners = boxLocal.cornerVertices(pModelInstance->pTransform->m_transform);
				for (const auto& corner : corners) {
					for (int i = 0; i < 3; ++i) {
						if (corner[i] < boxWorld.minVertex()[i]) boxWorld.minVertex()[i] = corner[i];
						if (corner[i] > boxWorld.maxVertex()[i]) boxWorld.maxVertex()[i] = corner[i];
					}
				}
				const auto worldCorners = boxWorld.cornerVertices();
				for (const auto& corner : worldCorners) {
					vVertices.push_back(corner[0]);
					vVertices.push_back(corner[1]);
					vVertices.push_back(corner[2]);
				}
			}
		}
		{
			ScopedTimeMeasurement m([](std::chrono::nanoseconds dur) {
				std::cout << "Took " << dur.count() << "ns\n";
			});
			m_kdTree.construct(vVertices);
		}
		for (const auto& [ent, inst] : m_mInstanceBuffer) {
			m_kdTree.emplace(glm::vec3{ inst.pTransform->m_transform * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f } }, ent);
		}
	}
};

}