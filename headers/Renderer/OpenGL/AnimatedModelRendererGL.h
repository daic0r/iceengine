//
//  AnimatedModelRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef AnimatedModelRendererGL_h
#define AnimatedModelRendererGL_h

#ifdef RENDERER_OPEN_GL

#include <Renderer/OpenGL/ModelRendererGL.h>

namespace Ice {

class IShaderConfigurator;

class AnimatedModelRendererGL : public ModelRendererGL {
	std::vector<glm::mat4> m_vJointTransforms;

public:
    AnimatedModelRendererGL() noexcept;
    //std::unique_ptr<IShaderConfigurator> createShaderConfigurator() const override;
};

}

#endif

#endif /* AnimatedModelRendererGL_h */
