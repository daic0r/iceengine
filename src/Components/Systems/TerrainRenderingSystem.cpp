//
//  TerrainRenderingSystem.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/Systems/TerrainRenderingSystem.h>
#include <System/SystemServices.h>
#include <Entities/EntityManager.h>
#include <World/Terrain.h>

namespace Ice {

TerrainRenderingSystem::TerrainRenderingSystem() noexcept {
    m_pRenderer = systemServices.getTerrainRenderer();
}

void TerrainRenderingSystem::render(const RenderEnvironment& env) noexcept {
    std::vector<Terrain> vTerrains;
    for (Entity e : entities(entityManager.currentScene()))
    {
        Terrain t;
        t.pTerrain = &entityManager.getComponent<TerrainComponent>(e);
        t.pMesh = &entityManager.getComponent<MeshComponent>(e);
        //t.pTexture = &entityManager.getComponent<TextureComponent>(e);
        vTerrains.emplace_back(t);
    }
    
    m_pRenderer->render(env, vTerrains);
}

}
