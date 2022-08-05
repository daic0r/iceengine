//
//  WaterShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WaterShaderConfigurator_h
#define WaterShaderConfigurator_h

#include  "LightShaderConfigurator.h"

namespace Ice {

class WaterShaderConfigurator : public LightShaderConfigurator {
    static const std::string m_strReflectionTextureUniformName;
    static const std::string m_strRefractionTextureUniformName;
    static const std::string m_strdudvMapUniformName;
    static const std::string m_strNormalMapUniformName;
    static const std::string m_strDepthMapUniformName;
    static const std::string m_strMoveFactorUniformName;
    static const std::string m_strCameraPosUniformName;

    int m_nReflectionTextureUniformId{ -1 };
    int m_nRefractionTextureUniformId{ -1 };
    int m_nDudvMapUniformId{ -1 };
    int m_nNormalMapUniformId{ -1 };
    int m_nDepthMapUniformId{ -1 };
    int m_nMoveFactorUniformId{ -1 };
    int m_nCameraPosUniformId{ -1 };

    void bindAttributes() const noexcept override;
    void getUniformLocations() noexcept override;
    
public:
    
    inline auto reflectionTextureId() const noexcept { return m_nReflectionTextureUniformId; }
    inline auto refractionTextureId() const noexcept { return m_nRefractionTextureUniformId; }
    void connectTextureUnits() const noexcept;
    void loadMoveFactor(float) const noexcept;
    void loadCameraPosition(const glm::vec3&) const noexcept;
};

}

#endif /* WaterShaderConfigurator_h */
