#ifndef BeeSystem_h
#define BeeSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/BeeComponent.h>
#include <Components/TransformComponent.h>

namespace Ice
{
    class BiomeSystem;
    class TerrainSystem;

    class BeeSystem : public EntityComponentSystem<BeeComponent, TransformComponent> {
        BiomeSystem* m_pBiomeSystem{};
        TerrainSystem* m_pTerrainSystem{};

    public:
        void onSystemsInitialized() noexcept override;
        bool update(float fDeltaTime) noexcept override;

        Entity createBee(std::uint16_t nBiomeID);

    };
    
} // namespace Ice


#endif