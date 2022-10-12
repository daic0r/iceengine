//
//  Path.h
//  IceCraft
//
//  Created by Matthias Gruen on 21.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Path_h
#define Path_h

#include <IceEngine.h>
#include <vector>
#include <System/Types.h>
#include <Entities/Entity.h>
#include <glm/vec3.hpp>
#include <functional>

namespace Ice {

class PathSegment;

class Path {
private:
    static std::uint32_t nNextId;

    std::uint32_t m_nId{ 0 };
    mutable float m_fLength{ -1.0f };

    // Direction specifies the side to which the preceding segment connects to the segment
    // (i.e. the first one will have NONE as its direction)
    std::vector<std::pair<OrthogonalDirection, Entity>> m_vPathSegments;

public:
    
    Path() noexcept;
    Path(const Path&) = default;
    float length() const;
    glm::vec3 getPointAt(float, glm::vec3* = nullptr) const;
    float getStationAtSegment(Entity) const noexcept;
	Entity getSegmentAtStation(float) const;
    auto pathId() const noexcept { return m_nId; }
    void setPathSegments(const decltype(m_vPathSegments)& vSegs);
    const decltype(m_vPathSegments)& pathSegments() const noexcept { return m_vPathSegments; }
    Entity startEntity() const { return m_vPathSegments.cbegin()->second; }
    Entity endEntity() const { return m_vPathSegments.at(m_vPathSegments.size() - 1).second; }
    void traverseSegmentsPairwise(const std::function<bool(Entity, const PathSegment&, Entity, const PathSegment&, OrthogonalDirection, OrthogonalDirection, float, float, float)>&) const noexcept;
    void append(const Path&);
};

}


#endif /* Path_h */
