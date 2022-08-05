//
//  PathAnimationComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathAnimationComponent_h
#define PathAnimationComponent_h

#include <Components/ComponentIds.h>
#include <Entities/Entity.h>
#include <Components/GraphNodeComponent.h>

namespace Ice {

struct PathAnimationComponent {
    inline static constexpr EntityComponentId id() noexcept { return PATH_ANIMATION_COMPONENT; }
    
    //std::uint32_t m_pathId{ 0 };
    std::vector<std::pair<Entity, const bool>> m_vPaths;    // Path entity, inverse?
    float m_fSpeed{ 0.0f };
    float m_fCurrentPosition{ 0.0f };   // path-local position (relative to path at current index)
    float m_fTargetPosition{ 0.0f };    // target position on last path
    std::uint16_t m_nCurrentPathIdx{ 0 };
	GraphNodeComponent::Type m_nodeType;
    bool m_bFirstFrame{ true };         // is this the first frame we're processing this animation?
};

}

#endif /* PathAnimationComponent_h */
