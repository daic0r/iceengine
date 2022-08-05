#pragma once

#include <Components/ComponentIds.h>

namespace Ice {

struct AnimatedModelInstanceTagComponent {

	static constexpr EntityComponentId id() noexcept { return ANIMATED_MODEL_INSTANCE_TAG_COMPONENT; }
};

}