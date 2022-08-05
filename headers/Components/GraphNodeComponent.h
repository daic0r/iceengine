//
//  GraphNodeComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 29.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef GraphNodeComponent_h
#define GraphNodeComponent_h

#include <Entities/Entity.h>
#include <vector>
#include <ostream>
#include <istream>
#include <sstream>
#include <System/StreamTools.h>
#include <Utils/serializables.h>

namespace Ice {

struct GraphNodeComponent {
    static constexpr EntityComponentId id() noexcept { return GRAPH_NODE_COMPONENT; }
    
    enum class Type : std::uint8_t {
		NONE,
        PEDESTRIAN,
        VEHICLE
    };

    std::vector<Entity> m_vEdges;   // Connected edges
    Type m_type{ Type::PEDESTRIAN };

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(type, m_type)
    END_DECLARE_SERIALIZABLE_ATTRIBS()

};

inline std::ostream& operator<<(std::ostream& os, const GraphNodeComponent& c) {
    os << GraphNodeComponent::id() << " ";
    switch (c.m_type) {
        case GraphNodeComponent::Type::PEDESTRIAN:
            os << "pedestrian";
            break;
        case GraphNodeComponent::Type::VEHICLE:
            os << "vehicle";
            break;
    }
    return os;
}

inline std::istream& operator>>(std::istream& is, GraphNodeComponent& c) {
	c.m_vEdges.clear();
    std::string t;
    is >> t;
    if (t == "pedestrian")
        c.m_type = GraphNodeComponent::Type::PEDESTRIAN;
    else
    if (t == "vehicle")
        c.m_type = GraphNodeComponent::Type::VEHICLE;
    else
        throw std::runtime_error("Invalid node type");
    
    return is;
}

}

#endif /* GraphNodeComponent_h */
