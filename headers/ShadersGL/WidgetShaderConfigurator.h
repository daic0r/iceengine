//
//  WidgetShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WidgetShaderConfigurator_h
#define WidgetShaderConfigurator_h

#include <ShadersGL/UIElementShaderConfigurator.h>
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace Ice {

class IShaderProgram;

class WidgetShaderConfigurator : public UIElementShaderConfigurator {

    static const std::string_view m_strPositionUniformName;
    static const std::string_view m_strScaleFactorsUniformName;
    static const std::string_view m_strAlignOffsetUniformName;
    static const std::string_view m_strColorUniformName;
    static const std::string_view m_strForegroundColorUniformName;
    static const std::string_view m_strMouseOverUniformName;
    static const std::string_view m_strIsTexturedUniformName;
    static const std::string_view m_strTextureUniformName;
    static const std::string_view m_strAspectRatioUniformName;
    static inline const std::string_view m_strBorderColorUniformName{ "borderColor" };
    static inline const std::string_view m_strBorderThicknessUniformName{ "borderThickness" };
    static inline const std::string_view m_strCornerRadiusUniformName{ "cornerRadius" };

    int m_nPositionUniformId{ -1 };
    int m_nScaleFactorsUniformId{ -1 };
    int m_nAlignOffsetUniformId{ -1 };
    int m_nColorUniformId{ -1 };
    int m_nForegroundColorUniformId{ -1 };
    int m_nMouseOverUniformId{ -1 };
    int m_nIsTexturedUniformId{ -1 };
    int m_nTextureUniformId{ -1 };
    int m_nAspectRatioUniformId{ -1 };
    int m_nBorderColorUniformId{ -1 };
    int m_nBorderThicknessUniformId{ -1 };
    int m_nCornerRadiusUniformId{ -1 };
    
public:
    void bindAttributes() const noexcept override;
    void getUniformLocations() noexcept override;
    void loadUniforms(const RenderEnvironment &) noexcept override {}
    
    void loadPosition(const glm::vec2&) noexcept;
    void loadScaleFactors(const glm::vec2&) noexcept;
    void loadAlignOffset(float, float) noexcept;
    void loadColor(const glm::vec4&) noexcept;
    void loadForegroundColor(const glm::vec4&) noexcept;
    void loadMouseOver(bool) noexcept;
    void loadIsTextured(bool) noexcept;
    void loadAspectRatio(float) noexcept;
    void loadBorderColor(const glm::vec4&) noexcept;
    void loadBorderThickness(float) noexcept;
    void loadCornerRadius(float) noexcept;
    
    inline int textureUniformId() const noexcept { return m_nTextureUniformId; }
    
};

};

#endif /* WidgetShaderConfigurator_h */
