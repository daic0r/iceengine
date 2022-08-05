//
//  TerrainTileSelectedEventArgs.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainTileSelectedEventArgs_h
#define TerrainTileSelectedEventArgs_h

#include "Event.h"

namespace Ice {

struct TerrainComponent;

class TerrainTileSelectedEventArgs : public IEventArgs {
    int m_nTileX{ -1 }, m_nTileZ{ -1 };
    const TerrainComponent& m_terrain;
    
public:
    
    TerrainTileSelectedEventArgs(int nTileX, int nTileZ, const TerrainComponent& comp) noexcept : m_nTileX{ nTileX }, m_nTileZ{ nTileZ }, m_terrain{ comp }  {}
    inline int tileX() const noexcept { return m_nTileX; }
    inline int tileZ() const noexcept { return m_nTileZ; }
    const auto& terrain() const noexcept { return m_terrain; }
};

}

#endif /* TerrainTileSelectedEventArgs_h */
