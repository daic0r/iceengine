//
//  RenderEnvironment.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef RenderEnvironment_h
#define RenderEnvironment_h

#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <optional>
#include <System/Types.h>
#include <Renderer/Frustum.h>

namespace Ice {

class PointLight;
class Camera;

struct RenderEnvironment {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec4 clipPlane;
    std::vector<PointLight*> lights;
	PointLight* pSun{ nullptr };
    
    Camera *pCamera{ nullptr };
    Frustum frustum;
    
    void* pMiscData{ nullptr };
    
    float fDeltaTime{ 0.0f };

    float fAspectRatio{ 0.0f };
    bool bWireframe{};
    bool bMainRenderPass{true};

    std::optional<float> fWaterLevel = std::nullopt; 
    TerrainClipMode clipMode = TerrainClipMode::NONE;
};

}

#endif /* RenderEnvironment_h */
