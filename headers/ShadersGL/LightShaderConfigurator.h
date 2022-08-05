//
//  LightShader.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef LightShader_h
#define LightShader_h

#include "Shader3dConfigurator.h"

namespace Ice {

class LightShaderConfigurator : public Shader3dConfigurator {
    static const std::string m_strLightPositionUniformName;
    static const std::string m_strLightColorUniformName;
    static const std::string m_strPlaneUniformName;

    int m_nLightPositionUniformId{ -1 };
    int m_nLightColorUniformId{ -1 };
    int m_nPlaneUniformId{ -1 };
    
public:
//    using Shader3d::Shader3d;
//    void bindAttributes() const noexcept override;
    void getUniformLocations() noexcept override;
    void loadUniforms(const RenderEnvironment& env) noexcept override;
    
    void loadLightPosition(const glm::vec3&) noexcept;
    void loadLightColor(float r, float g, float b) noexcept;
    void loadLightColor(const glm::vec3&) noexcept;
    void loadPlane(const glm::vec4&) noexcept;
};

}

#endif /* LightShader_h */
