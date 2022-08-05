//
//  WaterShaderConfigurator.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <ShadersGL/WaterShaderConfigurator.h>

namespace Ice {

const std::string WaterShaderConfigurator::m_strReflectionTextureUniformName{ "reflectionTexture" };
const std::string WaterShaderConfigurator::m_strRefractionTextureUniformName{ "refractionTexture" };
const std::string WaterShaderConfigurator::m_strdudvMapUniformName{ "dudvMap" };
const std::string WaterShaderConfigurator::m_strNormalMapUniformName{ "normalMap" };
const std::string WaterShaderConfigurator::m_strDepthMapUniformName{ "depthMap" };
const std::string WaterShaderConfigurator::m_strMoveFactorUniformName{ "moveFactor" };
const std::string WaterShaderConfigurator::m_strCameraPosUniformName{ "cameraPosition" };

void WaterShaderConfigurator::bindAttributes() const noexcept {
    
}

void WaterShaderConfigurator::getUniformLocations() noexcept {
    LightShaderConfigurator::getUniformLocations();
    m_nReflectionTextureUniformId = m_pShaderProgram->getUniformLocation(m_strReflectionTextureUniformName);
    m_nRefractionTextureUniformId = m_pShaderProgram->getUniformLocation(m_strRefractionTextureUniformName);
    m_nDudvMapUniformId = m_pShaderProgram->getUniformLocation(m_strdudvMapUniformName);
    m_nNormalMapUniformId = m_pShaderProgram->getUniformLocation(m_strNormalMapUniformName);
    m_nDepthMapUniformId = m_pShaderProgram->getUniformLocation(m_strDepthMapUniformName);
    m_nMoveFactorUniformId = m_pShaderProgram->getUniformLocation(m_strMoveFactorUniformName);
    m_nCameraPosUniformId = m_pShaderProgram->getUniformLocation(m_strCameraPosUniformName);
}

void WaterShaderConfigurator::connectTextureUnits() const noexcept {
    m_pShaderProgram->loadInt(m_nReflectionTextureUniformId, 0);
    m_pShaderProgram->loadInt(m_nRefractionTextureUniformId, 1);
    m_pShaderProgram->loadInt(m_nDudvMapUniformId, 2);
    m_pShaderProgram->loadInt(m_nNormalMapUniformId, 3);
    m_pShaderProgram->loadInt(m_nDepthMapUniformId, 4);
}

void WaterShaderConfigurator::loadMoveFactor(float fMoveFactor) const noexcept {
    m_pShaderProgram->loadFloat(m_nMoveFactorUniformId, fMoveFactor);
}

void WaterShaderConfigurator::loadCameraPosition(const glm::vec3& pos) const noexcept {
    m_pShaderProgram->loadVector3f(m_nCameraPosUniformId, pos);
}

}
