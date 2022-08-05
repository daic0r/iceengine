//
//  SkyboxShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 05.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SkyboxShader_h
#define SkyboxShader_h

#include "Shader3dConfigurator.h"

namespace Ice {

class SkyboxShaderConfigurator : public Shader3dConfigurator {
	static const std::string m_strRotationUniformName;

    int m_nRotationUniformId{ -1 };

public:
    //using Shader3d::Shader3d;
    void bindAttributes() const noexcept override;
	void getUniformLocations() noexcept override;
    void loadUniforms(const RenderEnvironment&) noexcept override;
	void loadRotation(float) noexcept;
};

}

#endif /* SkyboxShader_h */
