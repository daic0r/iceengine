//
//  PathComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 29.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathComponent_h
#define PathComponent_h

#include <Components/ComponentIds.h>
#include <System/Path.h>

namespace Ice {

struct PathComponent {

public:
    static constexpr EntityComponentId id() noexcept { return PATH_COMPONENT; }
    
    Path m_path;
};

}

#endif /* PathComponent_h */
