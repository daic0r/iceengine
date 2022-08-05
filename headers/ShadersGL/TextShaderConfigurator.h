//
//  TextShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 05.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TextShader_h
#define TextShader_h

#include <ShadersGL/UIElementShaderConfigurator.h>
#include <string>
#include <glm/vec4.hpp>

namespace Ice {

class IShaderProgram;

class TextShaderConfigurator : public UIElementShaderConfigurator {
    
    
    static const std::string m_strTextureUniformName;
    static const std::string m_strPositionUniformName;
    static const std::string m_strRotationUniformName;
    static const std::string m_strTextColorUniformName;
    static const std::string m_strBorderColorUniformName;
    static const std::string m_strThicknessUniformName;
    static const std::string m_strEdgeWidthUniformName;
    static const std::string m_strBorderWidthUniformName;
    static const std::string m_strBorderEdgeUniformName;
    static const std::string m_strShadowOffsetUniformName;
//    uniform vec3 borderColor;
//    uniform float thickness;
//    uniform float edgeWidth;
//    uniform float borderWidth;
//    uniform float borderEdge;
    
    int m_nTextureUniformId{ -1 };
    int m_nPositionUniformId{ -1 };
    int m_nRotationUniformId{ -1 };
    int m_nTextColorUniformId{ -1 };
    int m_nBorderColorUniformId{ -1 };
    int m_nThicknessUniformId{ -1 };
    int m_nEdgeWidthUniformId{ -1 };
    int m_nBorderWidthUniformId{ -1 };
    int m_nBorderEdgeUniformId{ -1 };
    int m_nShadowOffsetUniformId{ -1 };
    
public:
    //using ShaderProgram::ShaderProgram;
    void bindAttributes() const noexcept override;
    void getUniformLocations() noexcept override;
    void loadUniforms(const RenderEnvironment &) noexcept override {}
    
    void loadTextColor(const glm::vec4&) noexcept;
    void loadBorderColor(const glm::vec4&) noexcept;
    void loadThickness(float) noexcept;
    void loadEdgeWidth(float) noexcept;
    void loadBorderWidth(float) noexcept;
    void loadBorderEdge(float) noexcept;
    void loadShadowOffset(const glm::vec2&) noexcept;
    void loadRotation(float) noexcept;
    void loadPosition(const glm::vec2&) noexcept;
    
    inline int textureUniformId() const noexcept { return m_nTextureUniformId; }
    
};

}

#endif /* TextShader_h */
