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
        void setOriginalCanvas(IPostProcessingEffect* pOriginalCanvas) noexcept;
        bool update(float fDeltaTime) noexcept override;


        void setWaterLevel(float f) noexcept;
        auto waterLevel() const noexcept { return m_fWaterLevel; }
        void setGridSize(float f) noexcept;
    };
} // namespace Ice


#endif