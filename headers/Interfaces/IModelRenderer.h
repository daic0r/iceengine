//
//  IModelRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 14.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IModelRenderer_h
#define IModelRenderer_h

#include <unordered_map>
#include <vector>

namespace Ice {

struct Model;
struct ModelInstance;
struct RenderEnvironment;

class IModelRenderer {
public:
    //virtual void registerModel(Model* pModel) noexcept = 0;
    //virtual void prepareRendering(const RenderEnvironment&) noexcept = 0;
    virtual void render(const RenderEnvironment&, const std::vector<std::pair<Model, std::vector<ModelInstance*>>>&) noexcept = 0;
    //virtual void finishRendering() noexcept = 0;
	//virtual void renderShadowDepthTexture() const noexcept = 0;
    virtual ~IModelRenderer() = default; 
};

}

#endif /* IModelRenderer_h */
