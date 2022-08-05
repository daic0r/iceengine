//
//  TextStyle.h
//  IceCraft
//
//  Created by Matthias Gruen on 18.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TextStyle_h
#define TextStyle_h

#include <IceEngine.h>
#include <System/Types.h>
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>
#include <string>

namespace Ice {

namespace GUI {

struct FontInfo;

class ICEENGINE_API TextStyle {
    int m_nSize{ -1 };
    glm::vec4 m_color{ 1.0f, 1.0f, 1.0f, 1.0f };
    glm::vec4 m_borderColor{ 0.0f, 0.0f, 0.0f, 1.0f };
    std::string m_strFontName;
    
    float m_fThickness{ .44f };
    float m_fEdgeWidth{ .15f };
    float m_fBorderWidth{ 0.0f };
    float m_fBorderEdge{ 0.0f };
    glm::vec2 m_shadowOffset{ 0.0f, 0.0f };

    mutable const FontInfo* m_pFontInfo{ nullptr };

public:
    inline const auto& size() const noexcept { return m_nSize; }
    inline const auto& fontName() const noexcept { return m_strFontName; }
    inline const auto& color() const noexcept { return m_color; }
    inline const auto& borderColor() const noexcept { return m_borderColor; }
    inline const auto& thickness() const noexcept { return m_fThickness; }
    inline const auto& edgeWidth() const noexcept { return m_fEdgeWidth; }
    inline const auto& borderWidth() const noexcept { return m_fBorderWidth; }
    inline const auto& borderEdge() const noexcept { return m_fBorderEdge; }
    inline const auto& shadowOffset() const noexcept { return m_shadowOffset; }

    inline auto& size() noexcept { return m_nSize; }
    inline auto& fontName() noexcept { return m_strFontName; }
    inline auto& color() noexcept { return m_color; }
    inline auto& borderColor() noexcept { return m_borderColor; }
    inline auto& thickness() noexcept { return m_fThickness; }
    inline auto& edgeWidth() noexcept { return m_fEdgeWidth; }
    inline auto& borderWidth() noexcept { return m_fBorderWidth; }
    inline auto& borderEdge() noexcept { return m_fBorderEdge; }
    inline auto& shadowOffset() noexcept { return m_shadowOffset; }

    float getScaledPixelWidth() const noexcept;
    float getScaledPixelHeight() const noexcept;
    std::pair<float, float> getScaledPixelDimensions() const noexcept;
    float lineHeight() const noexcept;
    const FontInfo* fontInfo() const noexcept;
};

}

}

#endif /* TextStyle_h */
