//
//  Entity.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Entity_h
#define Entity_h

#include <cstdint>

namespace Ice {

using SceneId = std::uint16_t;
using EntityComponentId = std::uint16_t;
using Entity = std::uint64_t;

static constexpr Entity INVALID_ENTITY_ID = 0;

}

#endif /* Entity_h */
