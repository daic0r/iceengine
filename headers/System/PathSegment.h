//
//  PathSegment.h
//  IceCraft
//
//  Created by Matthias Gruen on 31.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef PathSegment_h
#define PathSegment_h

#include <IceEngine.h>
#include <System/Types.h>
#include <map>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <Utils/serializables.h>

namespace Ice {

class PathSegment {
    std::map<OrthogonalDirection, std::vector<glm::vec3>> m_mVertices;
    std::map<OrthogonalDirection, EdgeOrientation> m_mOrientations;
	std::map<OrthogonalDirection, std::vector<OrthogonalDirection>> m_mExclusions;
    mutable std::map<OrthogonalDirection, float> m_mLengths;
    
public:
    PathSegment() = default;
    PathSegment(const decltype(m_mVertices)&, const decltype(m_mOrientations)&, const decltype(m_mExclusions)&) noexcept;
    const auto& vertices() const noexcept { return m_mVertices; }
    
    const glm::vec3& startPoint(OrthogonalDirection) const;
    const glm::vec3& endPoint(OrthogonalDirection) const;
    bool connects(OrthogonalDirection dir, const PathSegment&) const noexcept;
    std::vector<OrthogonalDirection> connects(const PathSegment&) const noexcept;
    PathSegment transform(const glm::mat4&) const;
    float length(OrthogonalDirection) const;
    glm::vec3 getPointAt(OrthogonalDirection, float, glm::vec3* = nullptr) const;
    EdgeOrientation edgeOrientation(OrthogonalDirection) const;
	const auto& exclusions() const noexcept { return m_mExclusions; }

    friend void to_json(nlohmann::json& j, const PathSegment& p) {
        j.emplace("vertices", p.m_mVertices);
        j.emplace("orientations", p.m_mOrientations);
        j.emplace("exclusions", p.m_mExclusions);
    }

    friend void from_json(const nlohmann::json& j, PathSegment& p) {
        p.m_mVertices = j.at("vertices").get<std::map<OrthogonalDirection, std::vector<glm::vec3>>>();
        p.m_mOrientations = j.at("orientations").get<std::map<OrthogonalDirection, EdgeOrientation>>();
        p.m_mExclusions = j.at("exclusions").get<std::map<OrthogonalDirection, std::vector<OrthogonalDirection>>>();
    }
    
    friend std::ostream& operator<<(std::ostream&, const PathSegment&);
    friend std::istream& operator>>(std::istream&, PathSegment&);
};

 std::ostream& operator<<(std::ostream&, const PathSegment&);
 std::istream& operator>>(std::istream&, PathSegment&);

}

#endif /* PathSegment_h */
