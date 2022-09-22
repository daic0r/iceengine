#ifndef BiomeNodeComponent_h
#define BiomeNodeComponent_h

#include <Components/ComponentIds.h>
#include <System/RGBA.h>
#include <System/Percent.h>

namespace Ice
{
 
enum class BiomeType {
    NONE,
    GRASSLAND,
    SNOW
};
 
struct BiomeNodeComponent {
    enum class State {
        NONE,
        EXPANDING,
        SHRINKING,
        STATIC
    };
    static constexpr Ice::EntityComponentId id() noexcept { return ComponentIds::BIOME_NODE_COMPONENT; }

    static inline std::uint16_t NEXT_ID = 1;
    std::uint16_t nBiomeID{ NEXT_ID++ };
    BiomeType type;
    RGBA color;
    Percent power{100.0f};
    float fRadius{};
    State state{State::NONE};
};

} // namespace Ice

#endif