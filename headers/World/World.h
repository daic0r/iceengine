//
//  World.h
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef World_h
#define World_h

#include <IceEngine.h>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <Entities/Entity.h>
#include <System/Types.h>

namespace Ice {

class ICEENGINE_API World {
    std::vector<std::vector<Ice::Entity>> m_vTiles;
    std::uint16_t m_nWorldSize = 256;

public:

    World(std::uint16_t nDim);
    inline void setTile(int x, int z, Ice::Entity t) noexcept { m_vTiles[z][x] = t; }
    inline Ice::Entity tile(int x, int z) const noexcept { return m_vTiles[z][x]; }
    inline auto worldSize() const noexcept { return m_nWorldSize; }
    std::map<Ice::OrthogonalDirection, std::vector<Ice::Entity>> getAdjacentEntities(const std::pair<int,int>& fromTile, const std::pair<int,int>& toTile) const noexcept;
    
private:
    
    void initializeFromMap(const std::string&);
};

}

#endif /* World_h */
