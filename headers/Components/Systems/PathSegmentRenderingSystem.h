//
//  PathSegmentRenderingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathSegmentRenderingSystem_h
#define PathSegmentRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/PathSegmentComponent.h>
#include <Components/TransformComponent.h>

namespace Ice {

class IPathSegmentRenderer;

class PathSegmentRenderingSystem : public EntityComponentSystem<PathSegmentComponent, TransformComponent> {
    
    IPathSegmentRenderer* m_pRenderer{ nullptr };
    
public:
    
    PathSegmentRenderingSystem() noexcept;
    void render(const RenderEnvironment& env) noexcept;
};

}

#endif /* PathSegmentRenderingSystem_h */
