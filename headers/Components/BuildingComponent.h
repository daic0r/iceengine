#pragma once

#include <Components/ComponentIds.h>
#include <iostream>
#include <System/StreamTools.h>
#include <Utils/serializables.h>

namespace Ice {

struct BuildingComponent {
	static constexpr Ice::EntityComponentId id() noexcept { return BUILDING_COMPONENT; }

	int m_nCapacity{ 0 };		// number of people the building can house at any point in time
	std::string m_strName;
	
	BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
	SERIALIZABLE_ATTRIB(name, m_strName)
	SERIALIZABLE_ATTRIB(capacity, m_nCapacity)
	END_DECLARE_SERIALIZABLE_ATTRIBS()
};

inline std::ostream& operator<<(std::ostream& os, const BuildingComponent& comp) {
	os << BuildingComponent::id() << " ";
	Ice::StreamTools::writeStringToStream(os, comp.m_strName);
	os << " ";
	os << comp.m_nCapacity;

	return os;
}

inline std::istream& operator>>(std::istream& is, BuildingComponent& comp) {
	Ice::StreamTools::readStringFromStream(is, comp.m_strName);
	is >> comp.m_nCapacity;

	return is;
}

}
