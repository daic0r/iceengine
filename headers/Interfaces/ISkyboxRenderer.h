//
//  ISkyboxRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ISkyboxRenderer_h
#define ISkyboxRenderer_h

namespace Ice {

struct RenderEnvironment;

class ISkyboxRenderer {
public:
    virtual void render(const RenderEnvironment&, float fRotation) noexcept = 0;
    virtual ~ISkyboxRenderer() {}
};

}

#endif /* ISkyboxRenderer_h */
