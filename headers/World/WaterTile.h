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

namespace Ice {

class WaterTile {

    glm::vec2 m_position;
    float m_fHeight{ 0.0f };
    float m_fSize;
    
public:
    
    WaterTile(const glm::vec2& pos, float height, float fSize);
    
    inline auto height() const noexcept { return m_fHeight; }
    inline const auto& position() const noexcept { return m_position; }
    inline auto size() const noexcept { return m_fSize; };
};

}

#endif /* WaterTile_h */
