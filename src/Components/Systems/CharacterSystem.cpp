#include <Components/Systems/CharacterSystem.h>
#include <Components/TransformComponent.h>
#include <Components/ActionComponents/WalkActionComponent.h>
#include <Components/ActionComponents/PlaceBiomeNodeActionComponent.h>
#include <System/Math.h>
#include <Components/Systems/TerrainSystem.h>
#include <Components/Systems/BiomeSystem.h>
#include <Components/Systems/ModelAnimationSystem.h>
#include <Components/ModelReferenceComponent.h>
#include <Components/ModelAnimationComponent.h>
#include <Components/EntityHierarchyComponent.h>
#include <Algorithms/AStar.h>
#include <Components/Systems/SceneGraphSystem.h>

namespace Ice
{
    void CharacterSystem::onSystemsInitialized() noexcept {
        m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
        m_pBiomeSystem = entityManager.getSystem<BiomeSystem, true>();
        m_pSceneGraphSystem = entityManager.getSystem<SceneGraphSystem, true>();
    }

    bool CharacterSystem::doPlaceBiomeNode(Entity e) noexcept {
        auto& aniComp = entityManager.getComponent<ModelAnimationComponent>(e);
        if (aniComp.pCurrent == nullptr || aniComp.pCurrent != &aniComp.animations.at("Place")) {
            aniComp.pCurrent = &aniComp.animations.at("Place");
        }
        if (aniComp.pCurrent->currentTime() < aniComp.pCurrent->lastTime()) {
            aniComp.pCurrent = &aniComp.animations.at("Idle");
            const auto& action = entityManager.getComponent<PlaceBiomeNodeActionComponent>(e);
            auto biomeEnt = entityManager.createEntity();
            auto biomeTransform = entityManager.getComponent<EntityHierarchyComponent>(e);
            entityManager.addComponent(biomeEnt, biomeTransform);
            entityManager.addComponent(biomeEnt, BiomeNodeComponent{ .type = action.biomeType, .color = RGBA( 255, 255, 255 ), .power = 0.0_pct, .fRadius = 200.0f, .state = BiomeNodeComponent::State::EXPANDING });
            return true;
        }
        return false;
    }

    bool CharacterSystem::doWalk(Entity e, float fDeltaTime) noexcept {
        if (!entityManager.hasComponent<WalkActionComponent>(e))
            return true;

        if (Math::equal(fDeltaTime, 0.0f))
            return false;

        auto& walk = entityManager.getComponent<WalkActionComponent>(e);
        auto& trans = m_pSceneGraphSystem->getTransform(e, ComponentAccess::WRITE);

        glm::vec2 curPos{ trans[3][0], trans[3][2] };

        if (walk.vGridNodes.empty()) {
            walk.vGridNodes = m_pTerrainSystem->findPath(curPos.x, curPos.y, walk.target.x, walk.target.y);
        }
        auto curTarget = walk.vGridNodes.back();
        if (curPos == curTarget)
            return true;

        auto diff = glm::normalize(curTarget - curPos); // -> 1 unit per second
        float fEntAngle = acosf(diff.y) * Math::sgn(asinf(diff.x));
        diff *= 3.0f;
        /*
        if (Math::equal(glm::length(diff), 0.0f)) {
            return true;
        }
        */

        const auto bindTransform = glm::mat4{1.0f};
        bool bNext{};
        if (const auto fMissingLength = glm::length(curTarget - curPos); glm::length(diff * fDeltaTime) > fMissingLength) {
            diff = glm::normalize(diff) * fMissingLength;
            bNext = true;
        } else
            diff = diff * fDeltaTime;
        curPos += diff;
        /*
        trans.m_transform[3][0] += diff.x;
        trans.m_transform[3][2] += diff.y;
        */
        m_pTerrainSystem->getHeight(curPos.x, curPos.y, &trans);

        if (bNext) {
            walk.vGridNodes.pop_back();
        }
        trans = trans * glm::rotate(glm::mat4{1.0f}, fEntAngle, glm::vec3{0.0f, 1.0f, 0.0f}) * bindTransform;

        return walk.vGridNodes.empty();
    }

    bool CharacterSystem::update(float fDeltaTime) noexcept {
        for (auto e : entities(entityManager.currentScene())) {
            
            auto& character = entityManager.getComponent<CharacterComponent>(e);
            if (character.actions.empty())
                continue;
            const auto currentAction = character.actions.front();
            switch (currentAction) {
                case CharacterComponent::Action::WALK:
                    if (const auto bDone = doWalk(e, fDeltaTime); bDone) {
                        entityManager.removeComponent<WalkActionComponent>(e);
                        character.actions.pop_front();
                    }
                    break;
                case CharacterComponent::Action::PLACE_BIOME_NODE:
                    if (const auto bDone = doPlaceBiomeNode(e); bDone) {
                        entityManager.removeComponent<PlaceBiomeNodeActionComponent>(e);
                        character.actions.pop_front();
                    }
                    break;
                default:
                    character.actions.pop_front();
                    break;
            }
        }
        return true;
    }
} // namespace Ice
