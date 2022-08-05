//
//  UIElementShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef UIElementShaderConfigurator_h
#define UIElementShaderConfigurator_h

#include <Interfaces/IShaderConfigurator.h>
#include <string>
#include <glm/vec2.hpp>

namespace Ice {

class IShaderProgram;

class UIElementShaderConfigurator : public IShaderConfigurator {

    // For clipping in the shaders
    static inline const std::string m_strMinPointUniformName{ "minPoint" };
    static inline const std::string m_strMaxPointUniformName{ "maxPoint" };
    static inline const std::string m_strMinMaxTexCoorddUniformName{ "minMaxTexCoords" };

    int m_nMinPointUniformId{ -1 };
    int m_nMaxPointUniformId{ -1 };
    int m_nMinMaxTexCoordUniformId{ -1 };

protected:
    IShaderProgram *m_pShaderProgram{ nullptr };
 
public:
    void setShaderProgram(Ice::IShaderProgram *pProgram) noexcept override { m_pShaderProgram = pProgram; }
    void getUniformLocations() noexcept override;
    void loadUniforms(const RenderEnvironment &) noexcept override {}
 
    void loadMinPoint(const glm::vec2&) noexcept;
    void loadMaxPoint(const glm::vec2&) noexcept;
    void loadMinMaxTexCoord(const glm::vec2&, const glm::vec2&) noexcept;
};

}

#endif /* UIElementShaderConfigurator_h */
