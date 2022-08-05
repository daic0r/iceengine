#pragma once

#include <Components/ComponentIds.h>

namespace Ice {

struct ModelInstanceTagComponent {

	static constexpr EntityComponentId id() noexcept { return MODEL_INSTANCE_TAG_COMPONENT; }
};

}