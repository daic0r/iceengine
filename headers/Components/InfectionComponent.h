#pragma once

#include <Components/ComponentIds.h>

namespace Ice {

struct InfectionComponent {
	static constexpr EntityComponentId id() noexcept { return INFECTION_COMPONENT; }

};

}