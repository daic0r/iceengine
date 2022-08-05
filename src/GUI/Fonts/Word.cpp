//
//  Word.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 19.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Fonts/Word.h>
#include <System/SystemServices.h>
#include <Interfaces/IGraphicsSystem.h>
#include <GUI/Fonts/FontInfo.h>
#include <iostream>
#include <Interfaces/ITextManager.h>
#include <GUI/Fonts/TextStyle.h>

namespace Ice {

namespace GUI {

Word::Word(const std::string& str, const TextStyle* pStyle, float x, float y) noexcept
    : m_strWord{ str }, m_pStyle{ pStyle }, m_fX{ x }, m_fY{ y }
{
    m_pFontInfo = &systemServices.getTextManager()->getFont(pStyle->fontName());
}

float Word::width() const noexcept {
    if (m_fWidth < 0.0f) {
        const float fFactor = static_cast<float>(style()->size()) / static_cast<float>(fontInfo()->size);
        const float fPixelWidth = fFactor * 2.0f / systemServices.getGraphicsSystem()->displayWidth();
     
        m_fWidth = 0.0f;
        for (auto c : word()) {
            const auto& charInfo = fontInfo()->characters.at(c);
            const float fXAdvance = fPixelWidth * static_cast<float>(charInfo.xadvance);
            if (charInfo.height > m_nHeight)
                m_nHeight = charInfo.height;
            m_fWidth += fXAdvance;
        }
    }
    return m_fWidth;
}

void Word::generateVertices() {
    m_vVertexBuffer.clear();
    m_vVertexBuffer.reserve(24 * word().size());
    
    const float fFactor = static_cast<float>(style()->size()) / static_cast<float>(fontInfo()->size);
    
    auto pos = glm::vec2{ m_fX, m_fY };
    
    // Screen space goes from -1 to 1, hence the width is 2
    float fPixelWidth = 2.0f / systemServices.getGraphicsSystem()->displayWidth();
    float fPixelHeight = 2.0f / systemServices.getGraphicsSystem()->displayHeight();
    fPixelWidth *= fFactor;
    fPixelHeight *= fFactor;
        
    for (auto c : word()) {
        const auto& charInfo = fontInfo()->characters.at(c);
        
        // Only add vertices if this is not a space!
        if (c != 32) {
            addCharVertices(pos, charInfo, fPixelWidth, fPixelHeight);
        } else
            throw std::runtime_error("Words may not contain white spaces!");
        
        const float fXAdvance = fPixelWidth * static_cast<float>(charInfo.xadvance);
        pos.x += fXAdvance;
    }
}

void Word::addCharVertices(const glm::vec2& pos, const FontCharacterInfo& charInfo, float fPixelWidth, float fPixelHeight) noexcept {
    const float fLowY = static_cast<float>(pos.y) - (charInfo.yoffset * fPixelHeight) - (charInfo.height * fPixelHeight);
    const float fHighY = static_cast<float>(pos.y) - (charInfo.yoffset * fPixelHeight);
    const float fLowX = static_cast<float>(pos.x) + (charInfo.xoffset * fPixelWidth);
    const float fHighX = static_cast<float>(pos.x) + (charInfo.xoffset * fPixelWidth) + (charInfo.width * fPixelWidth);
    
    const float fTexLowX = charInfo.x/* + (fontInfo.padding.left * texW * fFactor)*/;
    const float fTexHighX = charInfo.x + charInfo.texWidth /*- (fontInfo.padding.right * texW* fFactor)*/;
    const float fTexLowY = charInfo.y - charInfo.texHeight /*- (fontInfo.padding.bottom * texH* fFactor)*/;
    const float fTexHighY = charInfo.y/* + (fontInfo.padding.top * texH * fFactor)*/;
    
    m_vVertexBuffer.emplace_back(fLowX);
    m_vVertexBuffer.emplace_back(fLowY);
    m_vVertexBuffer.emplace_back(fTexLowX);
    m_vVertexBuffer.emplace_back(fTexLowY);
    
    m_vVertexBuffer.emplace_back(fHighX);
    m_vVertexBuffer.emplace_back(fLowY);
    m_vVertexBuffer.emplace_back(fTexHighX);
    m_vVertexBuffer.emplace_back(fTexLowY);
    
    m_vVertexBuffer.emplace_back(fHighX);
    m_vVertexBuffer.emplace_back(fHighY);
    m_vVertexBuffer.emplace_back(fTexHighX);
    m_vVertexBuffer.emplace_back(fTexHighY);
    m_vVertexBuffer.emplace_back(fHighX);
    m_vVertexBuffer.emplace_back(fHighY);
    m_vVertexBuffer.emplace_back(fTexHighX);
    m_vVertexBuffer.emplace_back(fTexHighY);
    
    m_vVertexBuffer.emplace_back(fLowX);
    m_vVertexBuffer.emplace_back(fHighY);
    m_vVertexBuffer.emplace_back(fTexLowX);
    m_vVertexBuffer.emplace_back(fTexHighY);

    m_vVertexBuffer.emplace_back(fLowX);
    m_vVertexBuffer.emplace_back(fLowY);
    m_vVertexBuffer.emplace_back(fTexLowX);
    m_vVertexBuffer.emplace_back(fTexLowY);
}

}

}
