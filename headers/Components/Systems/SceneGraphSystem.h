#ifndef SceneGraphSystem_h
#define SceneGraphSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/EntityHierarchyComponent.h>
#include <System/AnimatedModel.h>
#include <System/AnimatedModelInstance.h>
#include <System/Octree.h>

namespace Ice
{
    class TerrainSystem;
    class ObjectRenderingSystem;
    class AnimatedModelRenderingSystem;
    class ABaseModelRenderingSystem;

    enum class ComponentAccess {
        READ,
        WRITE
    };
    
    enum class RenderSystem {
        STATIC,
        ANIMATED
    };

    class SceneGraphSystem : public EntityComponentSystem<EntityHierarchyComponent> {
        
        Entity findRoot(Entity) const;
        void updateEntity(Entity, const glm::mat4& matParentWorldTransform, bool bDirtyByInheritance);
 
    public:
        using modelstruct_t = std::variant<Model, AnimatedModel>;
        using modelinstance_t = std::variant<ModelInstance*, AnimatedModelInstance*>;
        struct TreeNodeContainer {
            std::vector<std::pair<ABaseModelRenderingSystem*, Entity>> m_vObjects;
            std::unordered_map<modelstruct_t, std::vector<modelinstance_t>> m_mModels;
        };

        struct TreeEmplaceValue {
            Entity m_ent;
            RenderSystem m_system;
            modelstruct_t m_model;
            modelinstance_t m_pInst;
        };
        using tree_t = Octree<TreeNodeContainer, TreeEmplaceValue>;

        SceneGraphSystem();
        void onSystemsInitialized() noexcept override;
        void onEntityAdded(Entity) noexcept override;
        glm::mat4& getTransform(Entity, ComponentAccess);
        bool update(float) override;
        int executionOrder() const noexcept override { return 10000; }
        auto& tree() noexcept { return m_tree; }
        void buildTree() noexcept;

    private:
        std::unordered_set<Entity> m_sDirtyRootEnts;
        tree_t m_tree{5};
        TerrainSystem* m_pTerrainSystem{};
        ObjectRenderingSystem* m_pObjectRenderingSystem{};
        AnimatedModelRenderingSystem* m_pAniModelRenderingSystem{};
    };
} // namespace Ice


#endif