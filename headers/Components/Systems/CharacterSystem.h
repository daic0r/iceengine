#ifndef CharacterSystem_h
#define CharacterSystem_h

#include <Components/ComponentIds.h>
#include <Entities/EntityComponentSystem.h>
#include <Components/CharacterComponent.h>

namespace Ice
{
    class TerrainSystem;

    class CharacterSystem : public EntityComponentSystem<CharacterComponent> {

        void doWalk(Entity e, float fDeltaTime) noexcept;

    public:
        bool update(float fDeltaTime) noexcept override;
        void onSystemsInitialized() noexcept override;

    private:
        TerrainSystem *m_pTerrainSystem{};
    };
} // namespace Ice


#endif