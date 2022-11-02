#ifndef SceneGraphSystem_h
#define SceneGraphSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/EntityHierarchyComponent.h>

namespace Ice
{
    enum class ComponentAccess {
        READ,
        WRITE
    };

    class SceneGraphSystem : public EntityComponentSystem<EntityHierarchyComponent> {
        
        Entity findRoot(Entity) const;
        void updateEntity(Entity, const glm::mat4& matParentWorldTransform, bool bDirtyByInheritance);

    public:
        void onEntityAdded(Entity) noexcept override;
        glm::mat4& getTransform(Entity, ComponentAccess);
        bool update(float) override;
        int executionOrder() const noexcept override { return 10000; }

    private:
        std::unordered_set<Entity> m_sDirtyRootEnts;
    };
} // namespace Ice


#endif