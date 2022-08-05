//
//  IMasterRenderer.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IMasterRenderer_h
#define IMasterRenderer_h

#include <glm/mat4x4.hpp>

namespace Ice {

struct Model;
class Camera;
class Scene;

class IMasterRenderer {
public:
    virtual bool init() = 0;
    virtual bool render(float fDeltaTime, Camera* pCamera) = 0;
    virtual void registerModel(Model* pModel) = 0;
    virtual void resizeWindow(int width, int height) noexcept = 0;
    virtual const glm::mat4& projectionMatrix() const noexcept = 0;
    virtual int displayWidth() const noexcept = 0;
    virtual int displayHeight() const noexcept = 0;
    virtual float fov() const noexcept = 0;
};

}

#endif /* IMasterRenderer_h */
