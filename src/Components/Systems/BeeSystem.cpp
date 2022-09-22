#include <Components/Systems/BeeSystem.h>
#include <Entities/EntityManager.h>
#include <Components/Systems/BiomeSystem.h>
#include <Components/Systems/TerrainSystem.h>
#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <System/Math.h>

namespace Ice
{

    void BeeSystem::onSystemsInitialized() noexcept {
        m_pBiomeSystem = entityManager.getSystem<BiomeSystem, true>();
        m_pTerrainSystem = entityManager.getSystem<TerrainSystem, false>();
    }

    bool BeeSystem::update(float fDeltaTime) noexcept {
        for (auto e : entities(entityManager.currentScene())) {
            auto& bee = entityManager.getComponent<BeeComponent>(e);
            auto& transf = entityManager.getComponent<TransformComponent>(e);
            const auto& biomeTransf = entityManager.getComponent<TransformComponent>(bee.nBiomeEnt);

            auto pos = glm::vec3{ biomeTransf.m_transform[3][0], 0.0f, biomeTransf.m_transform[3][2] };

            bee.fCurrentAngle = fmod(bee.fCurrentAngle + fDeltaTime, 2*M_PI);

            glm::mat4 terrainMat;
            auto dirChange = pos;
            pos.x += 15.0f * cosf(bee.fCurrentAngle);
            pos.z += 15.0f * sinf(bee.fCurrentAngle);

            dirChange = glm::normalize(pos - dirChange);
            float fEntAngle = acosf(dirChange.x) * Math::sgn(asinf(dirChange.z));

            transf.m_transform = glm::rotate(glm::mat4{1.0f}, -fEntAngle, glm::vec3{ 0.0f, 1.0f, 0.0f }) * bee.bindTransform;

            pos.y = m_pTerrainSystem->getHeight(pos.x, pos.z, &terrainMat);
            terrainMat[3][1] += 3.0f;
            transf.m_transform = terrainMat * transf.m_transform;
        }
        return true;
    }


    Entity BeeSystem::createBee(std::uint16_t nBiomeID) {
        auto e = entityManager.createEntity();

        auto biomeEnt = m_pBiomeSystem->findBiome(nBiomeID);
        entityManager.addComponent<BeeComponent>(e, BeeComponent{ .nBiomeID = nBiomeID, .nBiomeEnt = biomeEnt });

        return e;
    }
} // namespace Ice
