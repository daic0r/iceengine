//
//  ModelShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 25.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelShaderConfigurator_h
#define ModelShaderConfigurator_h

#include <SDL2/SDL_opengl.h>
#include <Interfaces/IShaderConfigurator.h>
#include <System/RenderMaterial.h>
#include <string>
#include <array>
#include <glm/mat4x4.hpp>
#include <vector>

namespace Ice {

class ShaderProgramGL;

class ModelShaderConfigurator : public IShaderConfigurator {
    static const std::string m_strProjMatrixUniformName;
    static const std::string m_strViewMatrixUniformName;
    //static const std::string m_strModelMatrixUniformName;
    static const std::string m_strTextureUniformName;
    
    static const std::string m_strMaterialAmbientUniformName;
    static const std::string m_strMaterialDiffuseUniformName;
    static const std::string m_strMaterialSpecularUniformName;
    static const std::string m_strMaterialSpecularExponentUniformName;
    static const std::string m_strMaterialAlphaUniformName;
    static const std::string m_strMaterialRefractionIndexUniformName;
    static const std::string m_strMaterialHasTextureUniformName;

	static const std::string m_strShadowProjViewMatrixUniformName;
	static const std::string m_strShadowMapTextureUniformName;
	static const std::string m_strShadowDistanceUniformName;
	static const std::string m_strShadowMarginUniformName;

	static const std::string m_strWaterLevelClipPlaneYUniformName;

//    static const std::vector<const GLchar*> MaterialUBOVarNames;
    
    GLint m_nProjectMatrixUniformId{ -1 };
    GLint m_nViewMatrixUniformId{ -1 };
    //GLint m_nModelMatrixUniformId{ -1 };
    GLint m_nTextureUniformId{ -1 };
    GLint m_nMaterialAmbientUniformId{ -1 };
    GLint m_nMaterialDiffuseUniformId{ -1 };
    GLint m_nMaterialSpecularUniformId{ -1 };
    GLint m_nMaterialSpecularExponentUniformId{ -1 };
    GLint m_nMaterialAlphaUniformId{ -1 };
    GLint m_nMaterialRefractionIndexUniformId{ -1 };
    GLint m_nMaterialHasTextureUniformId{ -1 };

	GLuint m_nCommonMatricesUBOIndex{ 0 };
	//GLint m_nShadowProjViewMatrixUniformId{ -1 };
	GLint m_nShadowMapTextureUniformId{ -1 };
	//GLint m_nShadowDistanceUniformId{ -1 };
	//GLint m_nShadowMarginUniformId{ -1 };
	GLint m_nWaterLevelClipPlaneYUniformId{ -1 };

	
//    std::vector<GLuint> m_vMaterialUBOVarOffsets;
//    std::unique_ptr<GLubyte[]> m_pMaterialUBOBuffer;
//    GLint m_nMaterialUBOBufferSize{ -1 };
//    GLuint m_nMaterialUBO{ 0 };

    RenderMaterial m_material;

    void loadProjectionMatrix(const glm::mat4&) const noexcept;
    void loadViewMatrix(const glm::mat4&) const noexcept;


protected:
    
    ShaderProgramGL *m_pShaderProgram{ nullptr };

public:
    ModelShaderConfigurator() = default;
    //ModelShaderConfigurator(ModelShaderConfigurator&&);
    ~ModelShaderConfigurator();
    void setShaderProgram(IShaderProgram *) noexcept override;
    void bindAttributes() const noexcept override;
    void initialize() noexcept override;
    void loadUniforms(const RenderEnvironment &) noexcept override;
    void getUniformLocations() noexcept override;
    void setRenderMaterial(const RenderMaterial&) noexcept;
    void loadTextureUnit(GLuint nUnit) const noexcept;
	void loadWaterLevelAndClipPlaneY(float fWaterLevel, int nClipPlaneY) noexcept;
	//void loadShadowProjViewMatrix(const glm::mat4&) const noexcept;
	//void loadShadowDistance(float) const noexcept;
	//void loadShadowMargin(float) const noexcept;
};

}

#endif /* ModelShaderConfigurator_h */
