#ifndef WaterRenderingSystem_h
#define WaterRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/WaterTileComponent.h>

namespace Ice 
{
    class IWaterRenderer;
    struct RenderEnvironment;
    class IPostProcessingEffect;

    class WaterRenderingSystem : public EntityComponentSystem<WaterTileComponent> {
         
        IWaterRenderer *m_pRenderer;
        std::vector<WaterTile*> m_vTiles;
        float m_fWaterLevel;
        
    public:
        void render(const RenderEnvironment&) noexcept;
        void onSystemsInitialized() noexcept override;
        void onEntityAdded(Entity) noexcept override;
        void setOriginalCanvas(IPostProcessingEffect* pOriginalCanvas) noexcept;

        void setWaterLevel(float f) noexcept;
        auto waterLevel() const noexcept { return m_fWaterLevel; }
    };
} // namespace Ice


#endif