#ifndef CharacterSystem_h
#define CharacterSystem_h

#include <Components/ComponentIds.h>
#include <Entities/EntityComponentSystem.h>
#include <Components/CharacterComponent.h>
#include <Components/BiomeNodeComponent.h>

namespace Ice
{
    class TerrainSystem;
    class BiomeSystem;

    class CharacterSystem : public EntityComponentSystem<CharacterComponent> {

        bool doWalk(Entity e, float fDeltaTime) noexcept; // return: done?
        void doPlaceBiomeNode(Entity e) noexcept;

    public:
        bool update(float fDeltaTime) noexcept override;
        void onSystemsInitialized() noexcept override;

    private:
        TerrainSystem *m_pTerrainSystem{};
        BiomeSystem *m_pBiomeSystem{};
    };
} // namespace Ice


#endif