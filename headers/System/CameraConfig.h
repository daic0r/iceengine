#ifndef CameraConfig_h
#define CameraConfig_h

#include <glm/vec3.hpp>
#include <System/Math.h>

namespace Ice
{
    struct CameraConfig {
        glm::vec3 m_lookAt{};
        float m_fMinPitch{ Math::degToRad(10.0f) };
        float m_fMaxPitch{ Math::degToRad(45.0f) };
        float m_fYaw{ 0.0f }, m_fPitch{ M_PI / 4.0f };
        float m_fMinDistance{ 10.0f };
        float m_fMaxDistance{ 50.0f };
        float m_fDistance{ 50.0f };
        float m_fYawAgility{ 10.0f };
        float m_fPitchAgility{ 10.0f };
        float m_fDistanceAgility{ 10.0f };
        float m_fLookAtAgility{ 10.0f };
        float m_fFoV{ M_PI / 4.0f };
    };
} // namespace Ice


#endif