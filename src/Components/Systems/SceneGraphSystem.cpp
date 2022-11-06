#include <Components/Systems/SceneGraphSystem.h>
#include <Components/TransformComponent.h>
#include <Components/Systems/TerrainSystem.h>
#include <Components/Systems/ObjectRenderingSystem.h>
#include <Components/Systems/AnimatedModelRenderingSystem.h>
#include <ranges>

namespace Ice
{
    SceneGraphSystem::SceneGraphSystem() {
        m_tree.setEmplaceFunc([this](TreeNodeContainer& container, const TreeEmplaceValue& value) {
            if (std::ranges::none_of(container.m_vObjects, [&value](auto&& v) { return v.second == value.m_ent; })) {
                container.m_vObjects.emplace_back(value.m_system == RenderSystem::STATIC ? static_cast<ABaseModelRenderingSystem*>(m_pObjectRenderingSystem) : static_cast<ABaseModelRenderingSystem*>(m_pAniModelRenderingSystem), value.m_ent);
                container.m_mModels[value.m_model].push_back(value.m_pInst);
            }
        });
    }

    void SceneGraphSystem::onSystemsInitialized() noexcept {
		m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
        m_pObjectRenderingSystem = entityManager.getSystem<ObjectRenderingSystem, true>();
        m_pAniModelRenderingSystem = entityManager.getSystem<AnimatedModelRenderingSystem, true>();

        m_tree.setGetVisibleObjectCollectionFunc([this](const SceneGraphSystem::TreeNodeContainer& container) {
            //this->m_vFrustumEnts.insert(this->m_vFrustumEnts.end(), container.m_vObjects.begin(), container.m_vObjects.end());
            for (const auto& [sys, ent] : container.m_vObjects) {
                sys->m_vFrustumEnts.push_back(ent);
            }
            for (const auto& [model, vInst] : container.m_mModels) {
                //auto pvInstances = std::holds_alternative<Model>(model) ? &m_pObjectRenderingSystem->m_vInstances : &m_pAniModelRenderingSystem->m_vInstances;
                auto pvInstances = std::visit(visitor{ [this](const Model& m) {
                    return &m_pObjectRenderingSystem->m_vInstances;
                }, [this](const AnimatedModel& m) {
                    return &m_pAniModelRenderingSystem->m_vInstances;
                } }, model);
                std::visit([&](auto&& m) -> decltype(auto) {
                    auto& v = ((*pvInstances).at(m));
                    std::ranges::transform(vInst, std::back_inserter(v), [](const modelinstance_t& inst) {
                        return std::visit([](auto&& i) {
                            return static_cast<ModelInstance*>(i);
                        }, inst);
                        //return sysInstPair.first == RenderSystem::STATIC ? std::get<ModelInstance>(sysInstPair.second) : std::get<AnimatedModelInstance>(sysInstPair.second);
                    });
                }, model);
            }
        });
    }

    void SceneGraphSystem::onEntityAdded(Entity e) noexcept {
        auto& comp = entityManager.getComponent<EntityHierarchyComponent>(e);
        if (comp.entRootEntity == INVALID_ENTITY_ID) {
            comp.entRootEntity = findRoot(e);
        }
        if (comp.entParent != INVALID_ENTITY_ID) {
            auto& parentComp = entityManager.getComponent<EntityHierarchyComponent>(comp.entParent);
            parentComp.vChildren.push_back(e);
        }
    }

    void SceneGraphSystem::buildTree() noexcept {
        const auto& worldExt = m_pTerrainSystem->worldExtents();
        const Extents3 ext3{ 
            glm::vec3{ worldExt.minPoint[0], -std::numeric_limits<float>::max(), worldExt.minPoint[1] },
            glm::vec3{ worldExt.maxPoint[0], std::numeric_limits<float>::max(), worldExt.maxPoint[1] }
        };
        const AABB worldBox{ ext3 };

        m_tree.construct(worldBox);
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
