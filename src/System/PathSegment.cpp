//
//  PathSegment.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 31.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/StreamTools.h>
#include <vector>
#include <map>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <System/Types.h>
#include <System/Tools.h>

#include <System/PathSegment.h>
#include <stdexcept>
#include <algorithm>

namespace Ice {

PathSegment::PathSegment(const decltype(m_mVertices)& mVerts, const decltype(m_mOrientations)& mOrients, const decltype(m_mExclusions)& mExclusions) noexcept {
    m_mVertices = mVerts;
    m_mOrientations = mOrients;
	m_mExclusions = mExclusions;
}

const glm::vec3& PathSegment::startPoint(OrthogonalDirection dir) const {
    if (m_mVertices.find(dir) == m_mVertices.end())
        throw std::runtime_error("Path cannot be connected to from the provided direction");
    
    if (m_mVertices.at(dir).size() < 2)
        throw std::runtime_error("Path segment needs at least 2 vertices");
    
    return m_mVertices.at(dir).at(0);
}

const glm::vec3& PathSegment::endPoint(OrthogonalDirection dir) const {
    if (m_mVertices.find(dir) == m_mVertices.end())
        throw std::runtime_error("Path cannot be connected to from the provided direction");
    
    if (m_mVertices.at(dir).size() < 2)
        throw std::runtime_error("Path segment needs at least 2 vertices");
    
    return m_mVertices.at(dir).back();
}

bool PathSegment::connects(OrthogonalDirection dir, const PathSegment& other) const noexcept {
    return m_mVertices.find(dir) != m_mVertices.cend() && other.m_mVertices.find(Tools::getOppositeDirection(dir)) != other.m_mVertices.cend();
}

// returns the directions on this segment that can connect to the passed in segment
std::vector<OrthogonalDirection> PathSegment::connects(const PathSegment& other) const noexcept {
    std::vector<OrthogonalDirection> vRet;
    for (const auto& [dir, vVerts] : m_mVertices) {
        const auto opposite = Tools::getOppositeDirection(dir);
        if (other.m_mVertices.find(opposite) != other.m_mVertices.cend() && other.edgeOrientation(opposite) == edgeOrientation(dir)) {
            const auto& endPointThis = endPoint(dir);
            const auto& startPointThis = startPoint(dir);
            const auto& endPointOther = other.endPoint(opposite);
            const auto& startPointOther = other.startPoint(opposite);
            if (glm::vec2{ endPointThis.x, endPointThis.z } == glm::vec2{ startPointOther.x, startPointOther.z } || glm::vec2{ startPointThis.x, startPointThis.z } == glm::vec2{ endPointOther.x, endPointOther.z }) {
                vRet.push_back(dir);
            }
        }
    }
    return vRet;
}

PathSegment PathSegment::transform(const glm::mat4& m) const {
    PathSegment p{ *this };
    std::map<OrthogonalDirection, OrthogonalDirection> mDirAssignments;
    for (auto& verts : p.m_mVertices) {
        auto axis = glm::vec4{ Tools::direction2axis(verts.first), 0.0f };
        axis = m * axis;
        auto newDir = mDirAssignments[verts.first] = Tools::axis2direction(glm::vec3{ axis });

        std::transform(verts.second.begin(), verts.second.end(), verts.second.begin(), [&m](const glm::vec3& vert) {
            return glm::vec3{ (m * glm::vec4{ vert.x, vert.y, vert.z, 1.0f }) };
        });

        if (newDir == Tools::getOppositeDirection(verts.first))
            std::reverse(verts.second.begin(), verts.second.end());
    }
    
    while (!mDirAssignments.empty()) {
        auto fromDir = mDirAssignments.begin()->first;
        auto toDir = mDirAssignments.begin()->second;
        if (fromDir == toDir) {
            mDirAssignments.erase(fromDir);
            continue;
        }
        if (p.m_mVertices.find(toDir) != p.m_mVertices.end()) {
            auto node_handle = p.m_mVertices.extract(fromDir);
            auto node_handle2 = p.m_mVertices.extract(toDir);
            std::swap(node_handle.key(), node_handle2.key());
            p.m_mVertices.insert(std::move(node_handle));
            p.m_mVertices.insert(std::move(node_handle2));
            mDirAssignments.erase(fromDir);
            mDirAssignments.erase(toDir);
        }
        else {
            auto node_handle = p.m_mVertices.extract(fromDir);
            node_handle.key() = toDir;
            p.m_mVertices.insert(std::move(node_handle));
            mDirAssignments.erase(fromDir);
        }
    }

    return p;
}

float PathSegment::length(OrthogonalDirection dir) const {
    auto lengthIter = m_mLengths.find(dir);
    if (lengthIter != m_mLengths.end())
        return lengthIter->second;
    float fRet{ 0.0f };
    auto iter = m_mVertices.find(dir);
    if (iter == m_mVertices.end())
        throw std::runtime_error("Path segment does not have the passed in direction");
    for (int i = 0; i < iter->second.size() - 1; ++i) {
        glm::vec3 dif = iter->second.at(i + 1) - iter->second.at(i);
        fRet += glm::length(dif);
    }
    m_mLengths[dir] = fRet;
    return fRet;
}

glm::vec3 PathSegment::getPointAt(OrthogonalDirection dir, float fStation, glm::vec3* pTangent) const {
    if (fStation < 0.0f || fStation > length(dir)) {
        throw std::runtime_error("Station must be between 0 and length!");
    }
    float fPrecedingLength{ 0.0f };
    auto iter = m_mVertices.find(dir);
    if (iter == m_mVertices.end())
        throw std::runtime_error("Path segment does not have the passed in direction");
    glm::vec3 vertA, vertB, vecAB;
    for (int i = 0; i < iter->second.size() - 1; ++i) {
        vertA = iter->second.at(i);
        vertB = iter->second.at(i + 1);
        vecAB = vertB - vertA;
        float fLen = glm::length(vecAB) ;
        if (fPrecedingLength <= fStation && fPrecedingLength + fLen > fStation) {
            fStation -= fPrecedingLength;
            break;
        }
        fPrecedingLength += fLen;
    }
    glm::vec3 fTangent = glm::normalize(vecAB);
    glm::vec3 retPoint = vertA + (fTangent * fStation);
    if (pTangent != nullptr)
        *pTangent = fTangent;
    return retPoint;
}

EdgeOrientation PathSegment::edgeOrientation(OrthogonalDirection dir) const {
    auto iter = m_mOrientations.find(dir);
    if (iter == m_mOrientations.end())
        throw std::runtime_error("Path segment does not have the passed in direction");
    
    return iter->second;
}

///////////////////////////////////////////////////////

std::ostream& operator<<(std::ostream& os, const PathSegment& p) {
    os << p.m_mVertices << COMPONENT_FIELD_SEPARATOR << p.m_mOrientations << COMPONENT_FIELD_SEPARATOR << p.m_mExclusions << COMPONENT_FIELD_SEPARATOR;
    return os;
}

std::istream& operator>>(std::istream& is, PathSegment& p) {
    p.m_mVertices.clear();
    //char chSep;
    is >> p.m_mVertices >> p.m_mOrientations;
	is >> p.m_mExclusions;

	for (auto& [dir, vec] : p.m_mVertices) {
		if (dir == OrthogonalDirection::LEFT) {
			if (vec.front().x > vec.back().x)
				std::reverse(vec.begin(), vec.end());
		} else if (dir == OrthogonalDirection::RIGHT) {
			if (vec.front().x > vec.back().x)
				std::reverse(vec.begin(), vec.end());
		} else if (dir == OrthogonalDirection::BOTTOM) {
			if (vec.front().z < vec.back().z)
				std::reverse(vec.begin(), vec.end());
		} else if (dir == OrthogonalDirection::TOP) {
			if (vec.front().z < vec.back().z)
				std::reverse(vec.begin(), vec.end());

		}
	}
    return is;
}

}
