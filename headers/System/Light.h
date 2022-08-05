//
//  PointLight.h
//  IceCraft
//
//  Created by Matthias Gruen on 06.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PointLight_h
#define PointLight_h

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace Ice {

class Frustum;

class PointLight {
    glm::vec3 m_position;
    glm::vec3 m_ambient;
	glm::vec3 m_color;
    
public:
    void setAmbient(const glm::vec3& ambient) noexcept { m_ambient = ambient; }
	const glm::vec3& ambient() const noexcept { return m_ambient; }
    void setColor(const glm::vec3& color) noexcept { m_color = color; }
    const glm::vec3& color() const noexcept { return m_color; }
    
    void setPosition(const glm::vec3& position) { m_position = position; }
    const glm::vec3& position() const noexcept { return m_position; }

	glm::mat4 getDirectionalLightViewMatrix(const Frustum& f) const noexcept;
};

}

#endif /* PointLight_h */
