//
//  ParticleShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ParticleShaderConfigurator_h
#define ParticleShaderConfigurator_h

#include "../Interfaces/IShaderProgram.h"
#include "../Interfaces/IShaderConfigurator.h"

namespace Ice {

class IShaderProgram;

class ParticleShaderConfigurator : public IShaderConfigurator {
    static const std::string m_strProjectionMatrixUniformName;
    static const std::string m_strParticleTextureUniformName;
    static const std::string m_strViewMatrixUniformName;
    
    static const std::string m_strCellDimsUniformName;

    int m_nProjectionMatrixUniformId{ -1 };
    int m_nParticleTextureUniformId{ -1 };
    int m_nCellDimsUniformId{ -1 };
    int m_nViewMatrixUniformId{ -1 };
    
    IShaderProgram *m_pShaderProgram{ nullptr };

public:
    void setShaderProgram(IShaderProgram* pProgram) noexcept override { m_pShaderProgram = pProgram; }
    void bindAttributes() const noexcept override;
    void loadUniforms(const RenderEnvironment&) noexcept override;
    void getUniformLocations() noexcept override;

    void loadProjectionMatrix(const glm::mat4&) noexcept;
    void loadCellDims(float, float) noexcept;
    void loadViewMatrix(const glm::mat4&) noexcept;
    inline int textureUniformId() const noexcept { return m_nParticleTextureUniformId; }
};

}

#endif /* ParticleShaderConfigurator_h */
