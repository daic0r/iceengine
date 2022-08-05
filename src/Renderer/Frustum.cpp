//
//  Frustum.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 30.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/Frustum.h>
#include <System/Camera.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <System/Tools.h>

namespace Ice {

Frustum::Frustum(const Camera& cam, float fDistNear, float fDistFar, float fovV, float fAspectRatio) {
    m_position = cam.position();
    m_direction = cam.direction();
    m_right = glm::normalize(glm::cross(cam.direction(), cam.up()));
    m_up = glm::normalize(glm::cross(m_right, m_direction));
    //m_right = glm::cross(cam.lookAt(), cam.up());
    const float fovH = fovV * fAspectRatio;
    const float fTanFovH2 = tan(fovH / 2.0f);
    const float fTanFovV2 = tan(fovV / 2.0f);
    const float fDistNear2 = 2.0f * fDistNear;
    const float fDistFar2 = 2.0f * fDistFar;
    m_fWidthNear = fDistNear2 * fTanFovH2;
    m_fHeightNear = fDistNear2 * fTanFovV2;
    m_fWidthFar = fDistFar2 * fTanFovH2;
    m_fHeightFar = fDistFar2 * fTanFovV2;
    m_fDistFar = fDistFar;
    m_fDistNear = fDistNear;
    
    auto centerFar = m_position + m_direction * m_fDistFar;
    const auto heightFar2 = m_fHeightFar / 2.0f;
    const auto widthFar2 = m_fWidthFar / 2.0f;
    const auto upHeightFar = m_up * heightFar2;
    const auto rightWidthFar = m_right * widthFar2;
    m_fbl = centerFar - upHeightFar - rightWidthFar;
    m_fbr = centerFar - upHeightFar + rightWidthFar;
    m_ftr = centerFar + upHeightFar + rightWidthFar;
    m_ftl = centerFar + upHeightFar - rightWidthFar;

    auto centerNear = m_position + m_direction * m_fDistNear;
    const auto heightNear2 = m_fHeightNear / 2.0f;
    const auto widthNear2 = m_fWidthNear / 2.0f;
    const auto upHeightNear = m_up * heightNear2;
    const auto rightWidthNear = m_right * widthNear2;
    m_nbl = centerNear - upHeightNear - rightWidthNear;
    m_nbr = centerNear - upHeightNear + rightWidthNear;
    m_ntr = centerNear + upHeightNear + rightWidthNear;
    m_ntl = centerNear + upHeightNear - rightWidthNear;

    
    m_planes.emplace(eFront, FrustumPlane{ centerNear, m_direction });
    m_planes.emplace(eBack, FrustumPlane{ centerFar, -m_direction });
    m_planes.emplace(eLeft, FrustumPlane{ m_ntl, m_nbl, m_ftl });
    m_planes.emplace(eRight, FrustumPlane{ m_ntr, m_ftr, m_nbr });
    m_planes.emplace(eTop, FrustumPlane{ m_ntr, m_ntl, m_ftr });
    m_planes.emplace(eBottom, FrustumPlane{ m_nbl , m_nbr, m_fbl });

}

Frustum::~Frustum() {
}

/*!
* \brief
* [Frustum::dist]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
float Frustum::dist(FaceDirection dir, const glm::vec3& pos) const
{
    return m_planes.at(dir).dist(pos);
}

const glm::vec3& Frustum::planeNormal(FaceDirection dir) const {
    return m_planes.at(dir).m_normal;
}

/*!
* \brief
* [Tools::checkMinBounds]
*
* bCheckMin: true = test minimum bounds point (n-vertex), false = check maximum bounds point (p-vertex)
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
bool Frustum::checkMinMaxBounds(const glm::vec3& posMin, const glm::vec3& posMax, bool bCheckMin) const noexcept
{
    std::vector<FaceDirection> vDirs;

    if (bCheckMin) {
        vDirs.emplace_back(eFront);
        vDirs.emplace_back(eRight);
        vDirs.emplace_back(eTop);
    } else {
        vDirs.emplace_back(eBack);
        vDirs.emplace_back(eLeft);
        vDirs.emplace_back(eBottom);
    }
    for (auto dir : vDirs) {
        auto bounds = bCheckMin ? posMax : posMin;
        const auto& normal = planeNormal(dir);
        
        // explanation: for the max bounds check for example, the box begins to be on the visible side of the plane
        // when the max point (i.e. the side facing the same direction as the plane's normal) is on its positive side
        // (i.e. the normal is positive). for example, if the max point is to the right of the left plane, the box begins
        // to be visible. for the min bounds check, the min point is closer to the right plane, so we're using the min point here.
        // once the min point is to the left of the right plane, the box begins to be visible. this is done for each dimension.
        for (int i = 0; i < 3; ++i) {
            if (normal[i] >= 0.0f)
                bounds[i] = bCheckMin ? posMin[i] : posMax[i];
        }
        const float fDist = dist(dir, bounds);
        if (fDist < 0.0f)
            return false;
    }

    return true;
}

glm::mat4 Frustum::getShadowProjection(const glm::mat4& sunViewMatrix, float fOffsetAtTheBack) const noexcept {
	// 1) Determine bounding points of the camera's frustum in sun/view space
	//	  so that it fits the camera's frustum as tightly as possible
	glm::vec3 minPoint, maxPoint;
	for (size_t i = 0; i < 3; ++i) {
		minPoint[i] = std::numeric_limits<float>::max();
		maxPoint[i] = -std::numeric_limits<float>::max();
	}

	const std::array<glm::vec4, 8> arCorners = {	
													sunViewMatrix * glm::vec4{ m_fbl, 1.0f }, sunViewMatrix * glm::vec4{ m_fbr, 1.0f }, sunViewMatrix * glm::vec4{ m_ftr, 1.0f }, sunViewMatrix * glm::vec4{ m_ftl, 1.0f },
													sunViewMatrix * glm::vec4{ m_nbl, 1.0f }, sunViewMatrix * glm::vec4{ m_nbr, 1.0f }, sunViewMatrix * glm::vec4{ m_ntr, 1.0f }, sunViewMatrix * glm::vec4{ m_ntl, 1.0f }
												};
	for (const auto& p : arCorners) {
		for (size_t i = 0; i < 3; ++i) {
			if (p[i] < minPoint[i])
				minPoint[i] = p[i];
			if (p[i] > maxPoint[i])
				maxPoint[i] = p[i];
		}
	}

	return glm::ortho(minPoint.x, maxPoint.x, minPoint.y, maxPoint.y, minPoint.z, maxPoint.z + fOffsetAtTheBack);
}

glm::vec3 Frustum::getCenter() const noexcept
{
	return m_position + m_direction * (m_fDistNear + (m_fDistFar - m_fDistNear) / 2.0f);
}

/*!
* \brief
* [Frustum::FrustumPlane::FrustumPlane]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
Frustum::FrustumPlane::FrustumPlane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
{
    auto v = p1 - p0;
    auto u = p2 - p0;
    m_point = p0;
    m_normal = glm::normalize(glm::cross(v, u));
    m_D = -glm::dot(p0, m_normal);
}

/*!
* \brief
* [Frustum::FrustumPlane::FrustumPlane]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
Frustum::FrustumPlane::FrustumPlane(const glm::vec3& p0, const glm::vec3& normal)
{
    m_normal = glm::normalize(normal);
    m_point = p0;
	m_D = -glm::dot(p0, m_normal);
}

/*!
* \brief
* [Frustum::FrustumPlane::dist]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
float Frustum::FrustumPlane::dist(const glm::vec3& p) const
{
    //float a = glm::dot(p - m_point, m_normal)/* + m_D*/;
    float b = glm::dot(p, m_normal) + m_D;
    return b;
}

/*!
* \brief
* [Frustum::FrustumPlane::print]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
void Frustum::FrustumPlane::print() const
{
    std::cout << "Normal: (" << m_normal.x << ", " << m_normal.y << ", " << m_normal.z << ")" << std::endl;
}

}
