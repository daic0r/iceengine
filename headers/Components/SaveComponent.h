//
//  SaveComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SaveComponent_h
#define SaveComponent_h

#include <Components/ComponentIds.h>

namespace Ice {

struct SaveComponent {
    inline static constexpr EntityComponentId id() noexcept { return SAVE_COMPONENT; }
};

}

#endif /* SaveComponent_h */
