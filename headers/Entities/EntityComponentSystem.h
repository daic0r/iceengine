//
//  EntityComponentSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef EntityComponentSystem_h
#define EntityComponentSystem_h

#include <Entities/EntityManager.h>
#include <Entities/BaseComponentSystem.h>

namespace Ice {

template<typename... Components> class EntityComponentSystem : public BaseComponentSystem {
public:
    EntityComponentSystem() noexcept;
};

}

#include <System/SystemServices.h>
#include <Components/ComponentIds.h>

namespace Ice {

template<typename... Components> EntityComponentSystem<Components...>::EntityComponentSystem() noexcept {
    componentIds() = { Components::id()... };
}

}

#endif /* EntityComponentSystem_h */
