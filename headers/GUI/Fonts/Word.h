//
//  Word.h
//  IceCraft
//
//  Created by Matthias Gruen on 19.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Word_h
#define Word_h

#include <vector>
#include <glm/vec2.hpp>
#include <string>

namespace Ice {

namespace GUI {

struct FontInfo;
struct FontCharacterInfo;
class TextStyle;

class Word {
    std::vector<float> m_vVertexBuffer;
    std::string m_strWord;
    mutable float m_fWidth{ -1.0f };
    mutable int m_nHeight{ 0 };
    float m_fX{ 0.0f }, m_fY{ 0.0f };
    const TextStyle* m_pStyle{ nullptr };
    const FontInfo *m_pFontInfo{ nullptr };
    
public:
    
    Word(const std::string& str, const TextStyle* pStyle, float x = 0.0f, float y = 0.0f) noexcept;
    void generateVertices();
    const auto& word() const noexcept { return m_strWord; }
    float width() const noexcept;
    const auto& vertexBuffer() const noexcept { return m_vVertexBuffer; }
    auto fontInfo() const noexcept { return m_pFontInfo; }
    auto style() const noexcept { return m_pStyle; }
    auto height() const noexcept { return m_nHeight; }
    
    auto posX() const noexcept { return m_fX; }
    auto posY() const noexcept { return m_fY; }
    void setPosX(float x) noexcept { m_fX = x; }
    void setPosY(float y) noexcept { m_fY = y; }
    
private:
    
    void addCharVertices(const glm::vec2& pos, const FontCharacterInfo& charInfo, float fPixelWidth, float fPixelHeight) noexcept;
};

}

}

#endif /* Word_h */
