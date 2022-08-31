//
//  IGraphicsSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IGraphicsSystem_h
#define IGraphicsSystem_h

#include <glm/mat4x4.hpp>

namespace Ice {

class IGraphicsSystem {
public:
    virtual void beginRender() noexcept = 0;
    virtual void endRender() noexcept = 0;
    virtual bool init() = 0;
    virtual void resizeWindow(int width, int height) noexcept = 0;
    virtual const glm::mat4& projectionMatrix() const noexcept = 0;
    virtual int displayWidth() const noexcept = 0;
    virtual int displayHeight() const noexcept = 0;
    virtual float fov() const noexcept = 0;
    virtual void setFov(float) noexcept = 0;
    virtual int screenWidth() const noexcept = 0;
    virtual int screenHeight() const noexcept = 0;
    virtual float getNormalizedPixelsHorizontal(int) const noexcept = 0;
    virtual float getNormalizedPixelsVertical(int) const noexcept = 0;
    virtual float getHorizontalPixelScale() const noexcept = 0;
    virtual float getVerticalPixelScale() const noexcept = 0;
	virtual float distNearPlane() const noexcept = 0;
	virtual float distFarPlane() const noexcept = 0;
	virtual float aspectRatio() const noexcept = 0;
    virtual void toggleWireframe() noexcept = 0;
    virtual bool wireframe() const noexcept = 0;
    virtual ~IGraphicsSystem() {}
};

}

#endif /* IGraphicsSystem_h */
