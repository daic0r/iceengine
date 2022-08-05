//
//  ITexture.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ITexture_h
#define ITexture_h

#include <string>
#include <utility>

namespace Ice {

class ITexture {
    int m_nWidth{ -1 }, m_nHeight{ -1 };
    float m_fTexelWidth{ -1.0f }, m_fTexelHeight{ -1.0f };
    int m_nRows{ -1 }, m_nCols{ -1 };
    mutable float m_fCellWidth{ -1.0f }, m_fCellHeight{ -1.0f };
    
public:
    virtual void loadFromFile(const std::string&) = 0;
    virtual void bind(int, unsigned int nTextureUnit = 0) const noexcept = 0;
    virtual void unbind() const noexcept = 0;
    
    inline void setWidth(int width) noexcept { m_nWidth = width; }
    inline void setHeight(int height) noexcept { m_nHeight = height; }
    inline void setTexelWidth(float width) noexcept { m_fTexelWidth = width; }
    inline void setTexelHeight(float height) noexcept { m_fTexelHeight = height; }
    inline void setRows(int nRows) noexcept { m_nRows = nRows; };
    inline void setCols(int nCols) noexcept { m_nCols = nCols; }
    
    inline float texelWidth() const noexcept { return m_fTexelWidth; }
    inline float texelHeight() const noexcept { return m_fTexelHeight; }
    inline int width() const noexcept { return m_nWidth; }
    inline int height() const noexcept { return m_nHeight; }
    inline int rows() const noexcept { return m_nRows; };
    inline int cols() const noexcept { return m_nCols; }
    inline float cellWidth() const noexcept {
        if (m_fCellWidth < 0.0f) {
            m_fCellWidth = static_cast<float>(width()) / static_cast<float>(cols()) * m_fTexelWidth;
        }
        return m_fCellWidth;
    }
    inline float cellHeight() const noexcept {
        if (m_fCellHeight < 0.0f) {
            m_fCellHeight = static_cast<float>(height()) / static_cast<float>(rows()) * m_fTexelHeight;
        }
        return m_fCellHeight;
    }
    virtual std::pair<float, float> getTexCoord(int x, int y) const noexcept {
        float outx = (float) x * texelWidth() + (texelWidth() / 2.0f);
        float outy = (float) y * texelHeight() + (texelHeight() / 2.0f);
        
        return std::make_pair(outx, outy);
    }
    
    virtual ~ITexture() {};
};

}

#endif /* ITexture_h */
