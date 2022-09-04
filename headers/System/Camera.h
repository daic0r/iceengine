//
//  Camera.h
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include <IceEngine.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <mutex>
#include <System/SmoothValue.h>
#include <System/CameraConfig.h>
#include <optional>
#include <functional>

namespace Ice {

class ICEENGINE_API Camera {
    friend class Controller;
    
    glm::vec3 m_position;
    glm::vec3 m_dir; 
    glm::vec3 m_up;
    float m_fFoV;
    
    // User configurable
    // #IDEA: Put these in derived class and put the above ones in BaseCamera class
    SmoothValue<glm::vec3> m_lookAt; // target point
    SmoothValue<float> m_fYaw{ 0.0f }, m_fPitch{ M_PI / 4.0 };
    SmoothValue<float> m_fDistance{ 50.0f };
    std::function<float(float,float)> m_getHeightFunc;

    std::optional<CameraConfig> m_config{};
    
public:
    //mutable std::recursive_mutex m_posMutex;
    Camera();

    Camera(const Camera& other) = default;
    Camera& operator=(const Camera&) = default;

    CameraConfig& config() noexcept;
    void initialize();

    void setUp(const glm::vec3& up) noexcept { m_up = glm::normalize(up); }
    void setDirection(const glm::vec3& dir) { m_dir = dir; }
    // User configurables
    void setFoV(float fov) noexcept { m_fFoV = fov; }
    void setYawAndPitch(float yaw, float pitch);
    void incYawAndPitch(float yaw, float pitch);
    void setLookAt(const glm::vec3& lookAt) noexcept { m_lookAt = lookAt; }
    void setYaw(float yaw) noexcept { m_fYaw = yaw; }
    void setPitch(float pitch) noexcept;
    void setDistance(float fDist) noexcept { m_fDistance = fDist; }
    void invertPitch() noexcept;
    
    const glm::vec3& position() const noexcept;
    auto& position() noexcept { return m_position; }
    auto up() const noexcept { return m_up; }
    auto lookAt() const noexcept { return m_lookAt.value(); }
    const auto& fov() const noexcept { return m_fFoV; }
    auto yaw() const noexcept { return m_fYaw.value(); }
    auto pitch() const noexcept { return m_fPitch.value(); }
    auto distance() const noexcept { return m_fDistance.value(); }
    auto direction() const noexcept { return m_dir; }
    
    glm::mat4 matrix() const noexcept;
    Camera getTransformedCopy(const glm::mat4&) const noexcept;
    
    void moveForward(float) noexcept;
    void moveSideways(float) noexcept;
    void zoom(float) noexcept;

    bool hasHeightGetterFunc() const noexcept { return (bool)m_getHeightFunc; }
    void setHeightGetterFunc(const std::function<float(float,float)>& func) { m_getHeightFunc = func; }

    void update(float fDeltaTime);

private:
    void setPosition(const glm::vec3& pos) noexcept;

};

}

#endif /* Camera_h */
