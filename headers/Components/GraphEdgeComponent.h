//
//  GraphEdgeComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 29.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef GraphEdgeComponent_h
#define GraphEdgeComponent_h

#include <Components/ComponentIds.h>
#include <Entities/Entity.h>
#include <iostream>

namespace Ice {

struct GraphEdgeComponent {
    static constexpr EntityComponentId id() noexcept { return GRAPH_EDGE_COMPONENT; }

    Entity m_startNode;
    Entity m_endNode;
    float m_fLength{ 0.0f };
//    Entity m_path;  // Path (geometrical representation of edge, consists of multiple path segments)
};

//static std::ostream& operator<<(std::ostream& os, const GraphEdgeComponent& c) {
//    //os << c.m_fLength << c.m_path;
//    os << GraphEdgeComponent::id() << " " << c.m_startNode << " " << c.m_endNode << " " << c.m_fLength;
//    return os;
//}
//
//static std::istream& operator>>(std::istream& is, GraphEdgeComponent& c) {
//    //is >> c.m_fLength >> c.m_path;
//    is >> c.m_startNode >> c.m_endNode >> c.m_fLength;
//    return is;
//}

}

#endif /* GraphEdgeComponent_h */
