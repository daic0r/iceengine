//
//  LocationComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef LocationComponent_h
#define LocationComponent_h

#include <Components/ComponentIds.h>
#include <Components/GraphNodeComponent.h>

namespace Ice {

struct LocationComponent {
    inline static constexpr EntityComponentId id() noexcept { return LOCATION_COMPONENT; }
    
	struct Location {
		Entity m_path{ INVALID_ENTITY_ID };
		float m_fStation{ -1.0f };
	};

	std::vector<std::pair<GraphNodeComponent::Type, Location>> m_locations;

	const Location& location(GraphNodeComponent::Type t) const {
		auto iter = std::find_if(m_locations.begin(), m_locations.end(), [t](const auto& kvp) {
			return t == kvp.first;
		});
		return iter->second;
	}

	Location& location(GraphNodeComponent::Type t) {
		return const_cast<Location&>(const_cast<const LocationComponent*>(this)->location(t));
	}

};

}


#endif /* LocationComponent_h */
