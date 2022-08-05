//
//  Frustum.h
//  IceCraft
//
//  Created by Matthias Gruen on 30.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Frustum_h
#define Frustum_h

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <System/Types.h>
#include <unordered_map>
#include <vector>

namespace Ice {

class Camera;

class Frustum {
	friend class ShadowFrustum;

	class FrustumPlane {
		friend class Frustum;

		glm::vec3 m_normal;
		float m_D;
		glm::vec3 m_point;

	public:
		FrustumPlane(const glm::vec3& p0, const glm::vec3& normal);
		FrustumPlane(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2);
		float dist(const glm::vec3& p) const;
		void print() const;
	};

	std::unordered_map<FaceDirection, FrustumPlane> m_planes;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	glm::vec3 m_up;
	glm::vec3 m_right;
	float m_fDistNear, m_fDistFar;
	float m_fWidthNear, m_fHeightNear;
	float m_fWidthFar, m_fHeightFar;

	glm::vec3 m_fbl, m_fbr, m_ftr, m_ftl; // far bottom left, etc
	glm::vec3 m_nbl, m_nbr, m_ntr, m_ntl; // near bottom left, etc

public:
	Frustum() = default;
	Frustum(const Camera& cam, float fDistNear, float fDistFar, float fovV, float fAspectRatio);
	~Frustum();

	const glm::vec3& position() const noexcept { return m_position; }
	const glm::vec3& direction() const noexcept { return m_direction; }
	float distNear() const noexcept { return m_fDistNear; }
	float distFar() const noexcept { return m_fDistFar; }

	float dist(FaceDirection, const glm::vec3&) const;
	const glm::vec3& planeNormal(FaceDirection) const;
	bool checkMinMaxBounds(const glm::vec3& posMin, const glm::vec3& posMax, bool bCheckMin) const noexcept;
	glm::mat4 getShadowProjection(const glm::mat4& sunViewMatrix, float fOffsetAtTheBack) const noexcept;
	glm::vec3 getCenter() const noexcept;

};

}

#endif /* Frustum_h */
