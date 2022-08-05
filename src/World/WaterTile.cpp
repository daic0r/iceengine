//
//  WaterTile.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <World/WaterTile.h>

namespace Ice {

WaterTile::WaterTile(const glm::vec2& pos, float height, float fSize) {
    m_position = pos;
    m_fHeight = height;
    m_fSize = fSize;
}

}
