//
//  Scene.h
//  IceCraft
//
//  Created by Matthias Gruen on 07.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Scene_h
#define Scene_h

#include <unordered_map>
#include <vector>
#include "../System/Light.h"
#include <glm/mat4x4.hpp>

namespace Ice {

struct Model;
class Camera;
class Particle;

struct ModelInstance;
class WaterTile;
struct Terrain;

class Scene {
    std::vector<PointLight*> m_vLights;
    std::unordered_map<Model*, std::vector<ModelInstance*>> m_mModelInstances;
    std::vector<Particle*> m_vParticles;
    std::vector<WaterTile*> m_vWaterTiles;
    std::vector<Terrain*> m_vTerrains;
    Camera *m_pCamera{ nullptr };
    glm::mat4 m_projectionMatrix;
    
public:
    
    Scene() = default;
    const auto& lights() const noexcept { return m_vLights; }
    auto& lights() noexcept { return m_vLights; }
    
    const auto& particles() const noexcept { return m_vParticles; }
    auto& particles() noexcept { return m_vParticles; }

    const auto& modelInstances() const noexcept { return m_mModelInstances; }
    auto& modelInstances() noexcept { return m_mModelInstances; }
    
    const auto& waterTiles() const noexcept { return m_vWaterTiles; }
    auto& waterTiles() noexcept { return m_vWaterTiles; }
    
    const auto& terrains() const noexcept { return m_vTerrains; }
    auto& terrains() noexcept { return m_vTerrains; }

    inline auto camera() const noexcept { return m_pCamera; }
    inline void setCamera(Camera* pCam) noexcept { m_pCamera = pCam; }
    inline const auto& projectionMatrix() const noexcept { return m_projectionMatrix; }
    inline void setProjectionMatrix(const glm::mat4& mat) noexcept { m_projectionMatrix = mat; }
        
    
    void addInstance(ModelInstance* pInst) noexcept;
    
};

}

#endif /* Scene_h */
