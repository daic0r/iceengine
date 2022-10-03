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

    void CharacterSystem::doWalk(Entity e, float fDeltaTime) noexcept {
        if (!entityManager.hasComponent<WalkActionComponent>(e) || Math::equal(fDeltaTime, 0.0f))
            return;

        auto& walk = entityManager.getComponent<WalkActionComponent>(e);
        auto& trans = entityManager.getComponent<TransformComponent>(e);

        glm::vec2 curPos{ trans.m_transform[3][0], trans.m_transform[3][2] };
        const auto diff = glm::normalize(walk.target - curPos) * fDeltaTime; // -> 1 unit per second
        if (Math::equal(glm::length(diff), 0.0f)) {
            entityManager.removeComponent<WalkActionComponent>(e);
            return;
        }

        trans.m_transform[3][0] += diff.x;
        trans.m_transform[3][2] += diff.y;
        trans.m_transform[3][1] = m_pTerrainSystem->getHeight(trans.m_transform[3][0], trans.m_transform[3][2]);
    }

    bool CharacterSystem::update(float fDeltaTime) noexcept {
        for (auto e : entities(entityManager.currentScene())) {
            
            auto& character = entityManager.getComponent<CharacterComponent>(e);
            switch (character.currentAction) {
                case CharacterComponent::Action::WALK:
                    doWalk(e, fDeltaTime);
                    break;
            }
        }
        return true;
    }
} // namespace Ice
