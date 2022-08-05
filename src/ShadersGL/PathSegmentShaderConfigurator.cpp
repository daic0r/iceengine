//
//  PathSegmentShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/PathSegmentShaderConfigurator.h>

namespace Ice {

void PathSegmentShaderConfigurator::bindAttributes() const noexcept {
    m_pShaderProgram->bindAttribute(0, "vertexPos");
}

}
