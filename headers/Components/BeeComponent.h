#ifndef BeeComponent_h
#define BeeComponent_h

#include <Components/ComponentIds.h>
#include <glm/mat4x4.hpp>

namespace Ice
{
    struct BeeComponent {

        static constexpr Ice::EntityComponentId id() noexcept { return ComponentIds::BEE_COMPONENT; }

        std::uint16_t nBiomeID{};
        Entity nBiomeEnt{};
        float fCurrentAngle{}; // bee flies in a circle, this is the angle
        glm::mat4 bindTransform; // original model-to-world transform
    };
} // namespace Ice


#endif