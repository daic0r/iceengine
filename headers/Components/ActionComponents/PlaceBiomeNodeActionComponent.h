#ifndef PlaceBiomeNodeActionComponent_h
#define PlaceBiomeNodeActionComponent_h

#include <Components/ComponentIds.h>
#include <Components/BiomeNodeComponent.h>

namespace Ice
{
    struct PlaceBiomeNodeActionComponent {
        static constexpr EntityComponentId id() noexcept { return PLACE_BIOME_NODE_ACTION_COMPONENT; }

        BiomeType biomeType;
    };
} // namespace Ice


#endif