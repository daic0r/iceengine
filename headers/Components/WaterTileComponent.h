#ifndef WaterTileComponent_h
#define WaterTileComponent_h

#include <World/WaterTile.h>
#include <Components/ComponentIds.h>

namespace Ice
{
    struct WaterTileComponent {
        static constexpr Ice::EntityComponentId id() noexcept { return ComponentIds::WATER_TILE_COMPONENT; }

        WaterTile m_tile;
    };
} // namespace Ice


#endif