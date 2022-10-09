//
//  ModelAnimationSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/ModelAnimationSystem.h>
#include <Entities/EntityManager.h>
#include <ModelAnimation/Joint.h>
#include <ModelAnimation/JointAnimation.h>
#include <iostream>
#include <glm/gtx/string_cast.hpp>

namespace Ice {

bool ModelAnimationSystem::update(float fDeltaTime) {
    
    for (auto e : entities(entityManager.currentScene())) {
        auto& animation = entityManager.getComponent<ModelAnimationComponent>(e);
        if (!animation.pCurrent)
            continue;

        auto& skeleton = entityManager.getComponent<SkeletonComponent>(e);
        
        animation.pCurrent->lastTime() = animation.pCurrent->currentTime();
        animation.pCurrent->currentTime() += fDeltaTime;
        animation.pCurrent->currentTime() = fmod(animation.pCurrent->currentTime(), animation.pCurrent->lengthSeconds());
        
        updateJoint(skeleton.m_rootJoint, *animation.pCurrent, skeleton.m_invBindShapeMatrix);
    }
    
    return true;
    
}

void ModelAnimationSystem::updateJoint(Joint& joint, const ModelAnimation& animation, const glm::mat4& parentTransform) {
    const auto& jAnimations = animation.jointAnimations();

    glm::mat4 currentTransform;
    auto jointAnimIter = jAnimations.find(joint.id());
    bool bHasAnimation = jointAnimIter != jAnimations.end();
    if (bHasAnimation) {
        // Get animation for this joint
        const auto& jAnim = jointAnimIter->second;
        const float fCurrentTime = animation.currentTime();
        const auto& animationJointTransforms = jAnim.jointTransforms();
        auto nextKeyFrameTransformIter = animationJointTransforms.upper_bound(fCurrentTime);
        if (nextKeyFrameTransformIter == animationJointTransforms.begin())
            return;
        auto prevKeyFrameTransformIter = nextKeyFrameTransformIter;
        --prevKeyFrameTransformIter;
     
        float fFrameProgress = (fCurrentTime - prevKeyFrameTransformIter->first) / (nextKeyFrameTransformIter->first - prevKeyFrameTransformIter->first);
        auto effectiveJointTransform = prevKeyFrameTransformIter->second.interpolate(nextKeyFrameTransformIter->second, fFrameProgress);
        currentTransform = parentTransform * effectiveJointTransform.toMatrix();
    } else {
        currentTransform = parentTransform * joint.bindTransform();
    }
    
    for (auto& childJoint : joint.children()) {
        updateJoint(childJoint, animation, currentTransform);
    }
    
    currentTransform = currentTransform * joint.invBindTransform();
    joint.setTransform(currentTransform);
    // ^^^
    // this is what the vertices will be transformed with. they're in model space, so transform them to local space
    // by multiplying with the inverse bind transform, then back to model space with the new transforms
}

}
