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

namespace Ice {

class PointLight;
class Camera;
class Frustum;

struct RenderEnvironment {
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::vec4 clipPlane;
    std::vector<PointLight*> lights;
	PointLight* pSun{ nullptr };
    
    Camera *pCamera{ nullptr };
    const Frustum *pFrustum{ nullptr };
    
    void* pMiscData{ nullptr };
    
    float fDeltaTime{ 0.0f };

    float fAspectRatio{ 0.0f };
};

}

#endif /* RenderEnvironment_h */
