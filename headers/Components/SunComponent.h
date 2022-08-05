#pragma once

#include <Components/ComponentIds.h>
#include <System/Light.h>

namespace Ice {

struct SunComponent {
	static constexpr EntityComponentId id() noexcept { return SUN_COMPONENT; }

	PointLight m_light;
};

}