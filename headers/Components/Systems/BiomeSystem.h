#ifndef BiomeSystem_h
#define BiomeSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/BiomeNodeComponent.h>
#include <Components/TransformComponent.h>
#include <array>

namespace Ice
{
    class TerrainSystem;
    class Triangle;

    class BiomeSystem : public EntityComponentSystem<BiomeNodeComponent, TransformComponent> {
    public:
        static constexpr std::size_t MAX_BIOMES_PER_POINT = 4;

        void onSystemsInitialized() noexcept;
        bool update(float fDeltaTime) noexcept override;
        std::array<std::pair<BiomeType, float>, MAX_BIOMES_PER_POINT> getBiomesAt(float x, float z) const; 
        std::array<std::pair<BiomeType, float>, MAX_BIOMES_PER_POINT> getBiomesAt(const Triangle& triangle, const std::array<float, 3>& bary) const; 

        Entity findBiome(std::uint16_t nId) const;

    private:
        TerrainSystem* m_pTerrainSystem{};
    };
} // namespace Ice


#endif