//
//  PathSegmentRenderingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Renderer/RenderEnvironment.h>
#include <Entities/EntityManager.h>
#include <Interfaces/IPathSegmentRenderer.h>

#include <Components/Systems/PathSegmentRenderingSystem.h>
#include <System/SystemServices.h>
#include <iostream>
#include <Components/TransformComponent.h>


namespace Ice {

PathSegmentRenderingSystem::PathSegmentRenderingSystem() noexcept {
    m_pRenderer = systemServices.getPathSegmentRenderer();
}

void PathSegmentRenderingSystem::render(const RenderEnvironment& env) noexcept {
    
    std::vector<Entity> vEnts;
    
    Entity prev{ 0 };
    for (auto e : entities(entityManager.currentScene())) {
        
        vEnts.push_back(e);
//        if (prev != 0 && prev != e) {
//            const auto& prevSegment = entityManager.getComponent<PathSegmentComponent>(prev);
//            const auto& prevTransf = entityManager.getComponent<TransformComponent>(prev);
//            const auto& segment = entityManager.getComponent<PathSegmentComponent>(e);
//            const auto& segmentTrans = entityManager.getComponent<TransformComponent>(e);
//
//            auto seg1 = prevSegment.m_pathSegment.transform(prevTransf.m_transform);
//            auto seg2 = segment.m_pathSegment.transform(segmentTrans.m_transform);
//            if (seg1.connects(seg2)) {
//                std::cout << "Connected" << std::endl;
//            }
//        }
        
        
        prev = e;
    }
    
    m_pRenderer->render(env, vEnts);
}

}
