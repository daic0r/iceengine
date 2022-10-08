#ifndef WalkActionComponent_h
#define WalkActionComponent_h

#include <Components/ComponentIds.h>
#include <glm/vec2.hpp>

namespace Ice
{
    struct WalkActionComponent {
        static constexpr EntityComponentId id() noexcept { return WALK_ACTION_COMPONENT; }

        glm::vec2 target{};
        std::vector<glm::vec2> vGridNodes;
    };
} // namespace Ice


#endif