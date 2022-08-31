//
//  WaterTile.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <World/WaterTile.h>

namespace Ice {

WaterTile::WaterTile(const glm::vec2& pos, float height, std::size_t nNumTilesH, std::size_t nNumTilesV, float fTileWidth, float fTileHeight)
    : m_position{ pos },
    m_fHeight{ height },
    m_nNumTilesH{ nNumTilesH },
    m_nNumTilesV{ nNumTilesV },
    m_fTileWidth{ fTileWidth },
    m_fTileHeight{ fTileHeight }
{
}

}
