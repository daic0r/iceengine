#pragma once

#include <IceEngine.h>
#include <vector>
#include <string>
#include <Entities/Entity.h>

namespace Ice {

class IComponentManager {
public:
	virtual std::vector<Entity> loadEntityFile(const std::string&) const = 0;
	virtual std::vector<Entity> loadEntityFileJson(const std::string&) const = 0;
	virtual ~IComponentManager() = default;
};

}