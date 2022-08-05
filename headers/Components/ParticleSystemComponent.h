#pragma once

#include <Components/ComponentIds.h>
#include <Particles/ParticleSystem.h>
#include <glm/vec3.hpp>

namespace Ice {

struct ParticleSystemComponent {
	static constexpr EntityComponentId id() noexcept { return PARTICLE_SYSTEM_COMPONENT; }

	ParticleSystem m_particleSystem;
	glm::vec3 m_positionOffset;
};

}