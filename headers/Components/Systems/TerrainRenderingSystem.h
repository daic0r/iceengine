//
//  TerrainRenderingSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainRenderingSystem_h
#define TerrainRenderingSystem_h

#include <Entities/EntityComponentSystem.h>
#include <Components/TerrainComponent.h>
#include <Components/MeshComponent.h>
#include <Components/TextureComponent.h>
#include <Renderer/RenderEnvironment.h>
#include <optional>
#include <Interfaces/ITerrainRenderer.h>

namespace Ice {

class ITerrainRenderer;

class TerrainRenderingSystem : public EntityComponentSystem<TerrainComponent, MeshComponent> {
    
    ITerrainRenderer *m_pRenderer;
    
public:
    TerrainRenderingSystem() noexcept;
    
    void render(const RenderEnvironment&) noexcept;
};

}

#endif /* TerrainRenderingSystem_h */
