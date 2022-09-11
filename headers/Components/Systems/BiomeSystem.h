#ifndef BiomeSystem_h
#define BiomeSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/BiomeNodeComponent.h>
#include <Components/TransformComponent.h>

namespace Ice
{
    class TerrainSystem;

    class BiomeSystem : public EntityComponentSystem<BiomeNodeComponent, TransformComponent> {
    public:

        void onSystemsInitialized() noexcept;
        bool update(float fDeltaTime) noexcept override;

    private:
        TerrainSystem* m_pTerrainSystem{};
    };
} // namespace Ice


#endif