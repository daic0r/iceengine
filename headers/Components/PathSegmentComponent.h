//
//  PathSegmentComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 29.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathSegmentComponent_h
#define PathSegmentComponent_h

#include <Entities/Entity.h>
#include <Components/ComponentIds.h>
#include <System/PathSegment.h>
#include <ostream>
#include <Utils/serializables.h>

namespace Ice {

struct PathSegmentComponent {
    static constexpr EntityComponentId id() noexcept { return PATH_SEGMENT_COMPONENT; }
    
    PathSegment m_pathSegment;
	Entity m_path{ INVALID_ENTITY_ID };  // what path do I belong to?

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(pathSegment, m_pathSegment)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

inline std::ostream& operator<<(std::ostream& os, const PathSegmentComponent& p) {
    os << PathSegmentComponent::id() << " " << /*p.m_path << " " <<*/ p.m_pathSegment;
    return os;
}

inline std::istream& operator>>(std::istream& is, PathSegmentComponent& p) {
    is >> /*p.m_path >>*/ p.m_pathSegment;
    return is;
}

}

#endif /* PathSegmentComponent_h */
