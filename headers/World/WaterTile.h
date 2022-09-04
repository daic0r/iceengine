//
//  WaterTile.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WaterTile_h
#define WaterTile_h

#include <glm/vec2.hpp>
#include <optional>

namespace Ice {

class WaterTile {

    glm::vec2 m_position;
    std::optional<float> m_fHeight{};
    float m_fTileWidth{}, m_fTileHeight;
    std::size_t m_nNumTilesH{}, m_nNumTilesV{};
    
public:
    
    WaterTile(const glm::vec2& pos, std::size_t nNumTilesH, std::size_t nNumTilesV, float fTileWidth, float fTileHeight, std::optional<float> height = std::nullopt);
    
    void setHeight(float f) noexcept { m_fHeight = f; }
    bool isHeightSet() const noexcept { return m_fHeight.has_value(); }
    inline auto height() const noexcept { return *m_fHeight; }
    inline const auto& position() const noexcept { return m_position; }
    inline auto horizontalSize() const noexcept { return m_fTileWidth * m_nNumTilesH; }
    inline auto verticalSize() const noexcept { return m_fTileHeight * m_nNumTilesV; }
    auto tileWidth() const noexcept { return m_fTileWidth; }
    auto tileHeight() const noexcept { return m_fTileHeight; }
    auto numTilesH() const noexcept { return m_nNumTilesH; }
    auto numTilesV() const noexcept { return m_nNumTilesV; }

};

}

#endif /* WaterTile_h */
