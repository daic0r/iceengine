#include <Components/Systems/SceneGraphSystem.h>
#include <Components/TransformComponent.h>

namespace Ice
{
    
    void SceneGraphSystem::onEntityAdded(Entity e) noexcept {
        /*
        const auto& comp = entityManager.getComponent<EntityHierarchyComponent>(e);
        if (comp.entRootEntity == INVALID_ENTITY_ID) {
            comp.entRootEntity = findRoot(e);
        }
        */
    }

    Entity SceneGraphSystem::findRoot(Entity e) const {
        EntityHierarchyComponent const* pComp = &entityManager.getComponent<EntityHierarchyComponent>(e);
        while (pComp->entParent != INVALID_ENTITY_ID) {
            e = pComp->entParent;
            pComp = &entityManager.getComponent<EntityHierarchyComponent>(e);
        }
        return e;
    }

    void SceneGraphSystem::updateEntity(Entity e, const glm::mat4& matParentWorldTransform, bool bDirtyByInheritance) {
        auto& comp = entityManager.getComponent<EntityHierarchyComponent>(e);   // this holds the transform relative to the parent
        auto& trans = entityManager.getComponent<TransformComponent>(e);

        if (comp.bDirty || bDirtyByInheritance) {
            trans.m_transform = matParentWorldTransform * comp.matTransform;
            comp.bDirty = false;
            bDirtyByInheritance = true;
        }

        for (auto entChild : comp.vChildren) {
            updateEntity(entChild, trans.m_transform, bDirtyByInheritance);
        }
    }

    bool SceneGraphSystem::update(float)  {
        for (auto e : m_sDirtyRootEnts) {
            updateEntity(e, glm::mat4{1.0f}, false);
        }
        m_sDirtyRootEnts.clear();
        return true;
    }

    glm::mat4& SceneGraphSystem::getTransform(Entity e, ComponentAccess access) {
        if (entities(entityManager.currentScene()).contains(e)) {
            auto& comp = entityManager.getComponent<EntityHierarchyComponent>(e);
            if (access == ComponentAccess::WRITE) {
                comp.bDirty = true;
                m_sDirtyRootEnts.insert(findRoot(e));
            }
            return comp.matTransform;
        }
        auto& trans = entityManager.getComponent<TransformComponent>(e);
        return trans.m_transform;
    }
} // namespace Ice
