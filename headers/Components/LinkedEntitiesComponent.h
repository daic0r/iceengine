//
//  LinkedEntitiesComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.04.21.
//  Copyright © 2021 Matthias Gruen. All rights reserved.
//

#ifndef LinkedEntitiesComponent_h
#define LinkedEntitiesComponent_h

#include <Components/ComponentIds.h>
#include <set>

namespace Ice {

struct LinkedEntitiesComponent {
    static constexpr EntityComponentId id() noexcept { return LINKED_ENTITIES_COMPONENT; }
    
    std::set<Entity> m_sLinkedEntities;
};

}


#endif /* LinkedEntitiesComponent_h */
