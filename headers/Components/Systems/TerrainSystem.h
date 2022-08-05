#ifndef TerrainSystem_h
#define TerrainSystem_h

#include <optional>
#include <Entities/EntityComponentSystem.h>
#include <Components/TerrainComponent.h>

namespace Ice
{
    class TerrainSystem : public EntityComponentSystem<TerrainComponent> {
    public:
        std::optional<float> getHeight(float x, float z);

    };
    
} // namespace Ice


#endif