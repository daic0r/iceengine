#pragma once

#include <istream>
#include <ostream>
#include <Components/ComponentIds.h>
#include <Utils/serializables.h>

namespace Ice {

struct BlueprintComponent {
	static constexpr EntityComponentId id() noexcept { return BLUEPRINT_COMPONENT; }

	BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
	END_DECLARE_SERIALIZABLE_ATTRIBS()	
};

inline std::ostream& operator<<(std::ostream& os, const BlueprintComponent&) {
	os << BlueprintComponent::id();
	return os;
}

inline std::istream& operator>>(std::istream& is, BlueprintComponent&) {
	return is;
}

}