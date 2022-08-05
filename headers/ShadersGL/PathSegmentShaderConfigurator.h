//
//  PathSegmentShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathSegmentShaderConfigurator_h
#define PathSegmentShaderConfigurator_h

#include <ShadersGL/Shader3dConfigurator.h>

namespace Ice {

class PathSegmentShaderConfigurator : public Shader3dConfigurator {
    void bindAttributes() const noexcept override;
};

}

#endif /* PathSegmentShaderConfigurator_h */
