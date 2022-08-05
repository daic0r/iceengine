//
//  CameraComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef CameraComponent_h
#define CameraComponent_h

#include <Components/ComponentIds.h>
#include <System/Camera.h>

namespace Ice {

struct CameraComponent {
    static constexpr EntityComponentId id() noexcept { return CAMERA_COMPONENT; }
    
    Camera m_camera;
    bool m_bPrimary{ false };
};

}

#endif /* CameraComponent_h */
