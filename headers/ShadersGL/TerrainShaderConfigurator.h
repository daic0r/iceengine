//
//  TerrainShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainShaderConfigurator_h
#define TerrainShaderConfigurator_h

#include <ShadersGL/Shader3dConfigurator.h>

namespace Ice {

class TerrainShaderConfigurator : public Shader3dConfigurator {
	static const std::string m_strShadowProjViewMatrixUniformName;
	static const std::string m_strShadowMapTextureUniformName;
	static const std::string m_strShadowDistanceUniformName;
	static const std::string m_strShadowMarginUniformName;

	GLuint m_nCommonMatricesUBOIndex{ 0 };
	//int m_nShadowProjViewMatrixUniformId{ -1 };
	int m_nShadowMapTextureUniformId{ -1 };
	//int m_nShadowDistanceUniformId{ -1 };
	//int m_nShadowMarginUniformId{ -1 };

    void bindAttributes() const noexcept override;
	void getUniformLocations() noexcept override;

public:
	//void loadShadowProjViewMatrix(const glm::mat4&) const noexcept;
	//void loadShadowDistance(float) const noexcept;
	//void loadShadowMargin(float) const noexcept;
};

}

#endif /* TerrainShaderConfigurator_h */
