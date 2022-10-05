#include <Components/Systems/CharacterSystem.h>
#include <Components/TransformComponent.h>
#include <Components/ActionComponents/WalkActionComponent.h>
#include <System/Math.h>
#include <Components/Systems/TerrainSystem.h>

namespace Ice
{
    void CharacterSystem::onSystemsInitialized() noexcept {
        m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
    }

    bool CharacterSystem::doWalk(Entity e, float fDeltaTime) noexcept {
        if (!entityManager.hasComponent<WalkActionComponent>(e))
            return true;

        if (Math::equal(fDeltaTime, 0.0f))
            return false;

        auto& walk = entityManager.getComponent<WalkActionComponent>(e);
        auto& trans = entityManager.getComponent<TransformComponent>(e);

        glm::vec2 curPos{ trans.m_transform[3][0], trans.m_transform[3][2] };
        auto diff = glm::normalize(walk.target - curPos); // -> 1 unit per second
        float fEntAngle = acosf(diff.x) * Math::sgn(asinf(diff.y));
        if (Math::equal(glm::length(diff), 0.0f)) {
            return true;
        }

        diff = diff * fDeltaTime;
        trans.m_transform[3][0] += diff.x;
        trans.m_transform[3][2] += diff.y;
        trans.m_transform[3][1] = m_pTerrainSystem->getHeight(trans.m_transform[3][0], trans.m_transform[3][2]);

        return false;
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
                default:
                    character.actions.pop_front();
                    break;
            }
        }
        return true;
    }
} // namespace Ice
