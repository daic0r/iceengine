#ifndef BiomeNodeComponent_h
#define BiomeNodeComponent_h

#include <Components/ComponentIds.h>
#include <System/RGBA.h>
#include <System/Percent.h>

namespace Ice
{
 
enum class BiomeType {
    NONE,
    GRASSLAND
};
 
struct BiomeNodeComponent {
    static constexpr Ice::EntityComponentId id() noexcept { return ComponentIds::BIOME_NODE_COMPONENT; }

    BiomeType m_type;
    RGBA m_color;
    Percent m_power;
    float m_fRadius{};
};

} // namespace Ice

#endif