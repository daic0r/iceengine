//
//  Shader3d.h
//  IceCraft
//
//  Created by Matthias Gruen on 05.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Shader3d_h
#define Shader3d_h

#include "ShaderProgramGL.h"
#include "../Interfaces/IShaderProgram.h"
#include "../Interfaces/IShaderConfigurator.h"

namespace Ice {

struct RenderEnvironment;

class Shader3dConfigurator : public IShaderConfigurator {
    static const std::string m_strProjMatrixUniformName;
    static const std::string m_strViewMatrixUniformName;
    static const std::string m_strModelMatrixUniformName;
    static const std::string m_strTextureUniformName;
    static const std::string m_strDiffuseUniformName;

    int m_nProjectMatrixUniformId{ -1 };
    int m_nViewMatrixUniformId{ -1 };
    int m_nModelMatrixUniformId{ -1 };
    int m_nTextureUniformId{ -1 };
    int m_nDiffuseUniformId{ -1 };
   

protected:
    ShaderProgramGL *m_pShaderProgram{ nullptr };
    void loadProjectionMatrix(const glm::mat4&);
    void loadViewMatrix(const glm::mat4&);

public:
    
//    using ShaderProgram::ShaderProgram;
//    Shader3d(Shader3d&&);
//    Shader3d& operator=(Shader3d&&);
    
    void setShaderProgram(IShaderProgram* pProgram) noexcept override { m_pShaderProgram = dynamic_cast<ShaderProgramGL*>(pProgram); }
    void loadModelMatrix(const glm::mat4&);
    void loadDiffuseColor(const glm::vec3&);
    void getUniformLocations() noexcept override;
    void loadUniforms(const RenderEnvironment&) noexcept override;
    
    int textureUniformId() const noexcept { return m_nTextureUniformId; }

};

}

#endif /* Shader3d_h */
