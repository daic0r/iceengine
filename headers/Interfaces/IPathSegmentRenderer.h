//
//  IPathSegmentRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IPathSegmentRenderer_h
#define IPathSegmentRenderer_h

namespace Ice {

struct RenderEnvironment;

class IPathSegmentRenderer {
public:
    virtual void render(const RenderEnvironment&, const std::vector<Entity>&) noexcept = 0;
    
    virtual ~IPathSegmentRenderer() {}
};

}


#endif /* IPathSegmentRenderer_h */
