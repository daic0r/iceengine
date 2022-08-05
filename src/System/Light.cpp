//
//  PointLight.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 06.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Light.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Renderer/Frustum.h>
#include <System/Tools.h>

namespace Ice {

// If this light serves as the sun, this function will compute the view matrix
// to render the scene from the light's perspective
glm::mat4 PointLight::getDirectionalLightViewMatrix(const Frustum& f) const noexcept {
	glm::vec3 lightDir{ -position() };
	lightDir = glm::normalize(lightDir);

	// Projection of the light direction onto the x-z plane
	// The length of this will be the cosine of the angle included
	// by the light's direction and the x-z plane (visualize this on the unit circle)
	const glm::vec2 xzProjection{ lightDir.x, lightDir.z };
	const float fProjLen = glm::length(xzProjection);
	// Since the length is the cosine of the angle, we can now use acosf to get the angle
	// Using the sign of the y-coordinate we determine whether we have to rotate counte-clockwise around the x-axis
	// (positive angle) or clockwise (negative angle); acosf will only return angles between 0 and 90 degrees which,
	// including the sign of y, is a total 180 degrees. The other 180 degrees don't need to be account for, this that
	// will be covered by a suitable yaw angle (170 degrees pitch for example is the same as 10 degrees pitch, followed by
	// a 180 degree yaw angle rotation)
	const float pitchAngle = Tools::sgn(lightDir.y) * acosf(fProjLen);
	// Yaw angle is determined using atan2f which should already take into account the different
	// combinations of + and -; tangent equals sin/cos, so we could also use asinf(lightDir.x) for example,
	// but this would require prior normalization of the lightDir vector, because this is currently the projection
	// of the light's direction on the x-z plane and therefore not normalized yet; the arc tangent only
	// requires the ratio between sin and cos, and therefore the values can be used directly
	//
	// First parameter is y-axis, second is x-axis. Yaw angle is rotation around the y-axis,
	// atan2f gives us the angle between a vector and the x-axis. Our projected 2d space is
	// y-axis = -x and x-axis = -z.
	const float yawAngle = atan2f(-lightDir.x, -lightDir.z); // lightDir.x = sin(angle around y-axis), lightDir.z = cos(angle around y-axis)
	// Center light view matrix at frustum center; it must be there, because in orthographic projection
	// the visible area is inside a cube centered around the origin, not inside a frustum in front of the
	// camera's position
	glm::mat4 ret = glm::translate(glm::mat4{ 1.0f }, f.getCenter());
	ret = glm::rotate(ret, yawAngle, glm::vec3{ 0.0f, 1.0f, 0.0f });
	ret = glm::rotate(ret, pitchAngle, glm::vec3{ 1.0f, 0.0f, 0.0f });
	// Using these values we actually describe how to get from light space to world space
	// Therefore invert the matrix here to get the world->light space transform that we want
	return glm::inverse(ret);
}

}