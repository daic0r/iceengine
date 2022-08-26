//
//  ObjectRenderingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/ObjectRenderingSystem.h>
#include <Components/Systems/ModelManagerSystem.h>
#include <Components/Systems/CameraControllerSystem.h>
#include <System/SystemServices.h>
#include <Components/MeshComponent.h>
#include <System/ModelInstance.h>
#include <System/Model.h>
#include <unordered_map>
#include <vector>
#include <Interfaces/IModelRenderer.h>
#include <Components/SkeletonComponent.h>
#include <System/Types.h>

namespace Ice {

ObjectRenderingSystem::ObjectRenderingSystem() noexcept {
    m_pRenderer = systemServices.getModelRenderer();
}

bool ObjectRenderingSystem::update(float fDeltaTime) noexcept {
	return BaseClassType::update(fDeltaTime, entities(entityManager.currentScene()));
}

void ObjectRenderingSystem::render(const RenderEnvironment& env) noexcept {
    
	BaseClassType::render(env, [](Entity, ModelInstance&) {});
    
    m_pRenderer->render(env, m_vInstances);
}

void ObjectRenderingSystem::onEntityAdded(Entity e) noexcept {
	BaseClassType::onEntityAdded(e);
}

void ObjectRenderingSystem::willRemoveComponent(Entity e, EntityComponentId id) noexcept {
	if (std::any_of(componentIds().cbegin(), componentIds().cend(), [id](EntityComponentId i) { return i == id; })) {
		BaseClassType::willRemoveComponent(e);
	}
}
	
void ObjectRenderingSystem::onSystemsInitialized() noexcept {
	BaseClassType::onSystemsInitialized();
}

bool ObjectRenderingSystem::isEntityEligibleForRendering(Entity e) const
{
	if (entityManager.hasComponent<SkeletonComponent>(e))
		return false;
	return true;
}

bool ObjectRenderingSystem::isEntityEligibleForFrustumCulling(Entity e) const
{
	if (entityManager.hasComponent<SkeletonComponent>(e))
		return false;
	return true;
}

Model ObjectRenderingSystem::makeModelStruct(Entity e) const
{
	auto& ref = entityManager.getComponent<ModelReferenceComponent>(e);
    auto& mesh = entityManager.getComponent<MeshComponent>(ref.m_entity);

	Model ret;
    ret.pMesh = &mesh;
	auto& materials = entityManager.getComponent<RenderMaterialsComponent>(ref.m_entity);
	ret.pMaterials = &materials;

	return ret;
}


}
