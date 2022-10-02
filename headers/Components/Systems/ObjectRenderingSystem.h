//
//  ObjectRenderingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ObjectRenderingSystem_h
#define ObjectRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/Systems/BaseModelRenderingSystem.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/ModelInstanceTagComponent.h>
#include <Components/TransformComponent.h>
#include <Renderer/RenderEnvironment.h>
#include <System/ModelInstance.h>
#include <System/Model.h>


namespace Ice {

class IModelRenderer;
struct Model;
struct ModelInstance;
class CameraControllerSystem;

class ObjectRenderingSystem final : 
	public EntityComponentSystem<ModelInstanceTagComponent, ModelReferenceComponent, TransformComponent>,
	public BaseModelRenderingSystem<Model, ModelInstance> {

	friend class MasterRenderingSystem;

	using BaseClassType = BaseModelRenderingSystem<Model, ModelInstance>;
 //   std::unordered_map<Model*, std::vector<ModelInstance*>> m_mInstances;
 //   std::map<MeshComponent*, Model> m_mModels;
	//std::map<Entity, ModelInstance> m_mInstanceBuffer;
    //std::vector<ModelInstance> vInstanceBuffer;

protected:
	Model makeModelStruct(Entity e) const override;
	bool isEntityEligibleForRendering(Entity e) const override;
	bool isEntityEligibleForFrustumCulling(Entity e) const override;

public:
    ObjectRenderingSystem() noexcept;

	bool update(float fDeltaTime) noexcept;
    void render(const RenderEnvironment&) noexcept;
	void onEntityAdded(Entity) noexcept;
	void willRemoveComponent(Entity, EntityComponentId) noexcept;
	void onSystemsInitialized() noexcept override;

	static Entity loadBlueprintFromExternalFile(std::string_view strFile);
	static Entity createInstance(Entity blueprint, const glm::mat4& transform);
	// Inherited via BaseModelRenderingSystem
};

}

#endif /* ObjectRenderingSystem_h */
