#ifndef EntityHierarchyComponent_h
#define EntityHierarchyComponent_h

#include <Components/ComponentIds.h>
#include <Entities/ComponentReference.h>

namespace Ice
{
    struct EntityHierarchyComponent {
        static constexpr EntityComponentId id() noexcept { return ENTITY_HIERARCHY_COMPONENT; }

        glm::mat4 matTransform;
        Entity entRootEntity{INVALID_ENTITY_ID};
        Entity entParent{INVALID_ENTITY_ID};
        std::vector<Entity> vChildren;
        bool bDirty{};
    };
    
} // namespace Ice


#endif
