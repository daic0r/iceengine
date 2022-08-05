#pragma once

#include <Entities/Entity.h>

namespace Ice {

class IEntityComponentBuffer {
public:
	virtual void onRemoveEntity(Entity, SceneId) noexcept = 0;
};

}
