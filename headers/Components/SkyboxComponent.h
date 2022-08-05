//
//  SkyboxComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SkyboxComponent_h
#define SkyboxComponent_h

#include <Components/ComponentIds.h>
#include <System/Skybox.h>

namespace Ice {

struct SkyboxComponent {
    static constexpr EntityComponentId id() noexcept { return SKYBOX_COMPONENT; }
};

}

#endif /* SkyboxComponent_h */
