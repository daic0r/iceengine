//
//  MeshGroupComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 14.04.21.
//  Copyright © 2021 Matthias Gruen. All rights reserved.
//

#ifndef MeshGroupComponent_h
#define MeshGroupComponent_h

#include <Components/ComponentIds.h>
#include <vector>
#include <iostream>
#include <System/StreamTools.h>
#include <Utils/serializables.h>

namespace Ice {

struct MeshGroupComponent {
    static constexpr EntityComponentId id() noexcept { return MESH_GROUP_COMPONENT; }
    
    std::vector<std::string> m_vMeshIds;
    std::vector<Entity> m_vEntities;

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(meshIds, m_vMeshIds)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

inline std::ostream& operator<<(std::ostream& os, const MeshGroupComponent& c) {
    os << MeshGroupComponent::id() << " ";
    os << c.m_vMeshIds << COMPONENT_FIELD_SEPARATOR;
    return os;
}

inline std::istream& operator>>(std::istream& is, MeshGroupComponent& c) {
    c.m_vMeshIds.clear();
    is >> c.m_vMeshIds;
    return is;
}

}

#endif /* MeshGroupComponent_h */
