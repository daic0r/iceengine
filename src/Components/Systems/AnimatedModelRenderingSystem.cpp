//
//  AnimatedModelRenderingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/AnimatedModelRenderingSystem.h>
#include <Entities/EntityManager.h>
#include <unordered_map>
#include <System/AnimatedModel.h>
#include <System/AnimatedModelInstance.h>
#include <Components/AnimatedMeshComponent.h>
#include <Renderer/RenderEnvironment.h>
#include <Interfaces/IModelRenderer.h>
#include <Components/ModelAnimationComponent.h>
#include <Components/SkeletonComponent.h>
#include <ModelAnimation/Joint.h>
#include <System/SystemServices.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace Ice {

AnimatedModelRenderingSystem::AnimatedModelRenderingSystem() noexcept {
    m_pRenderer = systemServices.getAnimatedModelRenderer();
}

void AnimatedModelRenderingSystem::render(const RenderEnvironment& env) noexcept {

	BaseClassType::render(env, [this](Entity e, AnimatedModelInstance& inst) {
		auto& ref = entityManager.getComponent<ModelReferenceComponent>(e);
		auto& skeleton = entityManager.getComponent<SkeletonComponent>(e);
		auto& aniMesh = entityManager.getComponent<AnimatedMeshComponent>(ref.m_entity);

		std::vector<glm::mat4> vJointTransforms;
		vJointTransforms.resize(aniMesh.m_nNumJoints);
		getJointTransforms(vJointTransforms, skeleton.m_rootJoint);
		inst.vJointTransforms = vJointTransforms;
	});
    
    m_pRenderer->render(env, m_vInstances);
}

void AnimatedModelRenderingSystem::onEntityAdded(Entity e) noexcept {
	BaseClassType::onEntityAdded(e);
}

void AnimatedModelRenderingSystem::willRemoveComponent(Entity e, EntityComponentId id) noexcept {
	if (std::any_of(componentIds().cbegin(), componentIds().cend(), [id](EntityComponentId i) { return i == id; })) {
		BaseClassType::willRemoveComponent(e);
	}
}

void AnimatedModelRenderingSystem::onSystemsInitialized() noexcept {
	BaseClassType::onSystemsInitialized();
}

bool AnimatedModelRenderingSystem::isEntityEligibleForRendering(Entity e) const
{
	if (!entityManager.hasComponent<SkeletonComponent>(e))
		return false;
	return true;
}

bool AnimatedModelRenderingSystem::isEntityEligibleForFrustumCulling(Entity e) const
{
	if (!entityManager.hasComponent<SkeletonComponent>(e))
		return false;
	return true;
}


bool AnimatedModelRenderingSystem::update(float fDeltaTime) noexcept {
	return BaseClassType::update(fDeltaTime, entities(entityManager.currentScene()));
}

void AnimatedModelRenderingSystem::getJointTransforms(std::vector<glm::mat4>& vOut, const Joint& j) const noexcept {
    vOut[j.id()] = j.transform();
    
    for (const auto& child : j.children()) {
        getJointTransforms(vOut, child);
    }
}

AnimatedModel AnimatedModelRenderingSystem::makeModelStruct(Entity e) const
{
	auto& ref = entityManager.getComponent<ModelReferenceComponent>(e);
	auto& mesh = entityManager.getComponent<MeshComponent>(ref.m_entity);
    auto& aniMesh = entityManager.getComponent<AnimatedMeshComponent>(ref.m_entity);
    auto& materials = entityManager.getComponent<RenderMaterialsComponent>(ref.m_entity);

    AnimatedModel mod;
    mod.pMesh = &mesh;
    mod.pMaterials = &materials;
    mod.pAnimatedMesh = &aniMesh;

	return mod;
}

}
