//
//  IWaterRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IWaterRenderer_h
#define IWaterRenderer_h

#include <vector>

namespace Ice {

struct RenderEnvironment;
class WaterTile;

class IWaterRenderer {
public:
    virtual void prepareRendering(const RenderEnvironment&) noexcept = 0;
    virtual void render(const RenderEnvironment&, const std::vector<WaterTile*>&) noexcept = 0;
    virtual void finishRendering() noexcept = 0;
    virtual ~IWaterRenderer() {}
};

}

#endif /* IWaterRenderer_h */