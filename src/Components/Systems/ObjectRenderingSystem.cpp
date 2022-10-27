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
#include <Importers/ModelImporterOBJ.h>
#include <Importers/ModelImporterGlTF.h>
#include <filesystem>
#include <System/File.h>

namespace Ice {

ObjectRenderingSystem::ObjectRenderingSystem() noexcept {
    m_pRenderer = systemServices.getModelRenderer();
}

bool ObjectRenderingSystem::update(float fDeltaTime) noexcept {
	return BaseClassType::update(fDeltaTime, entities(entityManager.currentScene()));
}

void ObjectRenderingSystem::render(const RenderEnvironment& env) noexcept {
    
	BaseClassType::render(env, nullptr);
    
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

Entity ObjectRenderingSystem::loadBlueprintFromExternalFile(std::string_view strFile) {
	namespace fs = std::filesystem;
	const auto path = fs::path{ strFile };
	if (path.extension() == ".obj") {
		auto model = ModelImporterOBJ{ AssetFile{ strFile} };
		std::map<std::string, MeshComponent> mMeshes;
		model.import(mMeshes);
		RenderMaterialsComponent mat;
		mat.materials() = model.materials();

		auto ent = entityManager.createEntity();
		entityManager.addComponent(ent, mMeshes.begin()->second);
		entityManager.addComponent(ent, mat);
		return ent;
	}
	else
	if (path.extension() == ".gltf" || path.extension() == ".glb") {
		auto model = ModelImporterGlTF{ AssetFile{ strFile }};
		model.import();

		auto ent = entityManager.createEntity();
		entityManager.addComponent(ent, model.meshes().begin()->second);
		RenderMaterialsComponent mat;
		mat.materials() = model.materials();
		entityManager.addComponent(ent, mat);
		return ent;
	}
	throw std::runtime_error("Unsupported file format");
}

Entity ObjectRenderingSystem::createInstance(Entity blueprint, const glm::mat4& transform) {
	auto ent = entityManager.createEntity();
	entityManager.addComponent(ent, TransformComponent{ transform });
	entityManager.addComponent(ent, ModelInstanceTagComponent{});
	entityManager.addComponent(ent, ModelReferenceComponent{ blueprint });
	return ent;
}

}
