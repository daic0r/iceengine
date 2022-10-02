//
//  AnimatedModelRenderingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedModelRenderingSystem_h
#define AnimatedModelRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/Systems/BaseModelRenderingSystem.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/AnimatedModelInstanceTagComponent.h>
#include <Components/ModelAnimationComponent.h>
#include <Components/TransformComponent.h>
#include <Components/SkeletonComponent.h>
#include <System/AnimatedModel.h>
#include <System/AnimatedModelInstance.h>

namespace Ice {

class IModelRenderer;
struct RenderEnvironment;
class Joint;

class AnimatedModelRenderingSystem : 
	public EntityComponentSystem<AnimatedModelInstanceTagComponent, ModelReferenceComponent, TransformComponent, SkeletonComponent>,
	public BaseModelRenderingSystem<AnimatedModel, AnimatedModelInstance>
{
	using BaseClassType = BaseModelRenderingSystem<AnimatedModel, AnimatedModelInstance>;
   
    void getJointTransforms(std::vector<glm::mat4>& vOut, const Joint&) const noexcept;

protected:
	AnimatedModel makeModelStruct(Entity e) const override;
	bool isEntityEligibleForRendering(Entity e) const override;
	bool isEntityEligibleForFrustumCulling(Entity e) const override;

public:
    
    AnimatedModelRenderingSystem() noexcept;
    void render(const RenderEnvironment&) noexcept;
	bool update(float fDeltaTime) noexcept;
	void onEntityAdded(Entity) noexcept;
	void willRemoveComponent(Entity, EntityComponentId) noexcept;
	void onSystemsInitialized() noexcept override;

	static Entity loadBlueprintFromExternalFile(std::string_view strFile);
	static Entity createInstance(Entity blueprint, const glm::mat4& transform);
};


}

#endif /* AnimatedModelRenderingSystem_h */
