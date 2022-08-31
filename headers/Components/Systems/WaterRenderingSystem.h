#ifndef WaterRenderingSystem_h
#define WaterRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/WaterTileComponent.h>

namespace Ice 
{
    class IWaterRenderer;
    struct RenderEnvironment;

    class WaterRenderingSystem : public EntityComponentSystem<WaterTileComponent> {
         
        IWaterRenderer *m_pRenderer;
        std::vector<WaterTile*> m_vTiles;
        
    public:
        void render(const RenderEnvironment&) noexcept;
        void onSystemsInitialized() noexcept override;
    };
} // namespace Ice


#endif