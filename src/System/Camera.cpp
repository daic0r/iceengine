//
//  Camera.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Camera.h>
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <System/Config.h>
#include <System/Tools.h>
#include <System/Math.h>
#include <System/SystemServices.h>
#include <glm/mat4x4.hpp>

namespace Ice {

CameraConfig& Camera::config() noexcept {
    return m_config.has_value() ? m_config.value() : systemServices.getConfig()->camera();
}

void Camera::initialize() {
    const auto& conf = config();
    m_fDistance.setAgility(conf.m_fDistanceAgility);
    m_fYaw.setAgility(conf.m_fYawAgility);
    m_fPitch.setAgility(conf.m_fPitchAgility);
    m_lookAt.setAgility(conf.m_fLookAtAgility);
    
    m_fDistance.force(conf.m_fDistance);
    m_fYaw.force(conf.m_fYaw);
    m_fPitch.force(conf.m_fPitch);
    m_lookAt.force(conf.m_lookAt);

    m_fFoV = conf.m_fFoV;
}

const glm::vec3& Camera::position() const noexcept {
    //std::lock_guard<std::recursive_mutex> guard{m_posMutex};
    return m_position;
}

void Camera::setPosition(const glm::vec3 &pos) noexcept {
    //std::lock_guard<std::recursive_mutex> guard{m_posMutex};
    m_position = pos;
}

Camera::Camera() {
    initialize();

    zoom(0.0f);
}

void Camera::moveForward(float dist) noexcept {
    auto flatDirection = direction();
    flatDirection.y = 0.0f;

    const auto shiftBy = flatDirection * dist;
    auto newTarget = m_lookAt.target() + shiftBy;
    newTarget.y = m_getHeightFunc(newTarget.x, newTarget.z);

    setLookAt(newTarget);
}

void Camera::moveSideways(float dist) noexcept {
    auto flatDirection = direction();
    flatDirection.y = 0.0f;
    flatDirection = glm::normalize(glm::cross(flatDirection, up()));

    const auto shiftBy = flatDirection * dist;
    auto newTarget = m_lookAt.target() + shiftBy;
    newTarget.y = m_getHeightFunc(newTarget.x, newTarget.z);

    setLookAt(newTarget);
}

void Camera::zoom(float factor) noexcept {
    const auto& conf = config();
    const auto newDistance = Math::clamp(m_fDistance.target() + factor, conf.m_fMinDistance, conf.m_fMaxDistance);
    setDistance(newDistance);
    // 45 degrees = dist = 50
    // 10 degrees = dist = 10
    /*
    setPitch(config().m_fMinPitch + 
        Math::map0to1Range(m_fDistance.target(), conf.m_fMinDistance, conf.m_fMaxDistance) * 
            (conf.m_fMaxPitch - conf.m_fMinPitch));
    */

}

glm::mat4 Camera::matrix() const noexcept {
    return glm::lookAt(position(), lookAt(), up());
}

Camera Camera::getTransformedCopy(const glm::mat4& m) const noexcept {
    Camera ret{ *this };
    
    ret.m_position = m * glm::vec4{ m_position, 1.0f };
    ret.m_up = m * glm::vec4{ m_up, 0.0f };
    ret.m_dir = m * glm::vec4{ m_dir, 0.0f };

    return ret;
}

void Camera::setPitch(float pitch) noexcept {
    const auto& conf = config();
    m_fPitch = Math::clamp(pitch, conf.m_fMinPitch, conf.m_fMaxPitch);
}

void Camera::setYawAndPitch(float yaw, float pitch) {
    setYaw(yaw);
    setPitch(pitch);
}

void Camera::incYawAndPitch(float yaw, float pitch) {
    setYawAndPitch(m_fYaw.target() + yaw, m_fPitch.target() + pitch);
}

void Camera::update(float fDeltaTime) {
    m_lookAt.update(fDeltaTime);
    m_fPitch.update(fDeltaTime);
    m_fYaw.update(fDeltaTime);
    m_fDistance.update(fDeltaTime);

    const auto toCam = glm::vec3{ 
        m_fDistance * cosf(m_fPitch) * sinf(m_fYaw),
        m_fDistance * sinf(m_fPitch),
        m_fDistance * cosf(m_fPitch) * cosf(m_fYaw) 
    };
    const auto up = glm::normalize(glm::vec3{ 
        cosf(m_fPitch + M_PI / 2.0f) * sinf(m_fYaw),
        sinf(m_fPitch + M_PI / 2.0f),
        cosf(m_fPitch + M_PI / 2.0f) * cosf(m_fYaw)
    });
    auto newPos = m_lookAt.value() + toCam;
    if (m_getHeightFunc) {
        const auto fHeight = m_getHeightFunc(newPos.x, newPos.z);
        if (fHeight > newPos.y) {
            newPos.y = fHeight + 1.0f;
        }
    }
    setPosition(newPos);
    setUp(up);
    setDirection(-glm::normalize(toCam));
}

void Camera::invertPitch() noexcept {
    m_fPitch.force(-m_fPitch);
    update(0.0f);
}

}
