//
//  Shader3d.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 05.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <ShadersGL/Shader3dConfigurator.h>
#include <Renderer/RenderEnvironment.h>

namespace Ice {

const std::string Shader3dConfigurator::m_strProjMatrixUniformName{ "projection" };
const std::string Shader3dConfigurator::m_strViewMatrixUniformName{ "view" };
const std::string Shader3dConfigurator::m_strModelMatrixUniformName{ "model" };
const std::string Shader3dConfigurator::m_strTextureUniformName{ "tex" };
const std::string Shader3dConfigurator::m_strDiffuseUniformName{ "diffuse" };


//Shader3d::Shader3d(Shader3d&& other) : ShaderProgramGL(std::move(other)) {
//    *this = std::move(other);
//}

//Shader3d& Shader3d::operator=(Shader3d &&other) {
//    //ShaderProgramGL::operator=(std::move(other));
//    m_nModelMatrixUniformId = other.m_nModelMatrixUniformId;
//    m_nViewMatrixUniformId = other.m_nViewMatrixUniformId;
//    m_nProjectMatrixUniformId = other.m_nProjectMatrixUniformId;
//    m_nTextureUniformId = other.m_nTextureUniformId;
//    other.m_nModelMatrixUniformId = other.m_nViewMatrixUniformId = other.m_nProjectMatrixUniformId = other.m_nTextureUniformId = -1;
//    return *this;
//}

void Shader3dConfigurator::getUniformLocations() noexcept {
    m_nProjectMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strProjMatrixUniformName);
    m_nViewMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strViewMatrixUniformName);
    m_nModelMatrixUniformId = m_pShaderProgram->getUniformLocation(m_strModelMatrixUniformName);
    m_nTextureUniformId = m_pShaderProgram->getUniformLocation(m_strTextureUniformName);
    m_nDiffuseUniformId = m_pShaderProgram->getUniformLocation(m_strDiffuseUniformName);
}

void Shader3dConfigurator::loadProjectionMatrix(const glm::mat4& mat) {
    if (m_nProjectMatrixUniformId != -1)
        m_pShaderProgram->loadMatrix4f(m_nProjectMatrixUniformId, mat);
}

void Shader3dConfigurator::loadViewMatrix(const glm::mat4& mat) {
    if (m_nViewMatrixUniformId != -1)
        m_pShaderProgram->loadMatrix4f(m_nViewMatrixUniformId, mat);
}

void Shader3dConfigurator::loadModelMatrix(const glm::mat4& mat) {
    if (m_nModelMatrixUniformId != -1)
        m_pShaderProgram->loadMatrix4f(m_nModelMatrixUniformId, mat);
}

void Shader3dConfigurator::loadDiffuseColor(const glm::vec3& c) {
    if (m_nDiffuseUniformId != -1)
        m_pShaderProgram->loadVector3f(m_nDiffuseUniformId, c);
}

void Shader3dConfigurator::loadUniforms(const RenderEnvironment &env) noexcept {
    loadViewMatrix(env.viewMatrix);
    loadProjectionMatrix(env.projectionMatrix);
}

}
