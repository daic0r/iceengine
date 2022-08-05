//
//  Path.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Path.h>
#include <Entities/EntityManager.h>
#include <Components/PathSegmentComponent.h>
#include <System/Tools.h>

namespace Ice {

std::uint32_t Path::nNextId = 0;

Path::Path() noexcept {
    m_nId = nNextId++;
}

float Path::length() const {
    if (m_fLength >= 0.0f)
        return m_fLength;
    
    m_fLength = 0.0f;
    for (int i = 1; i < m_vPathSegments.size(); ++i) {
        const auto& [dir, segEnt] = m_vPathSegments.at(i);
        const auto& [prevDir, prevSegEnt] = m_vPathSegments.at(i - 1);
        const auto& prevSeg = entityManager.getComponent<PathSegmentComponent>(prevSegEnt);
        const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(segEnt);
        m_fLength += thisSeg.m_pathSegment.length(dir);
        m_fLength += prevSeg.m_pathSegment.length(Tools::getOppositeDirection(dir));
    }
    return m_fLength;
}

glm::vec3 Path::getPointAt(float fStation, glm::vec3* pTangent) const {
    if (fStation < 0.0f || fStation > length())
        throw std::runtime_error("Station must be between 0 and length!");
    
    glm::vec3 retPoint;
    traverseSegmentsPairwise([&pTangent, fStation, &retPoint](Entity, const PathSegment& prevSeg, Entity, const PathSegment& thisSeg, OrthogonalDirection prevSegDir, OrthogonalDirection thisSegDir, float fPrevLen, float fLen, float fPrecedingLength)
        {
            if (fPrecedingLength <= fStation && fPrecedingLength + fPrevLen > fStation) {
                float fLocalStation = fStation - fPrecedingLength;
                const bool bInverse = prevSegDir == OrthogonalDirection::LEFT || prevSegDir == OrthogonalDirection::BOTTOM;
                if (bInverse) {
                    fLocalStation = fPrevLen - fLocalStation;
                }
                retPoint = prevSeg.getPointAt(prevSegDir, fLocalStation, pTangent);
                if (bInverse && pTangent != nullptr) {
                    *pTangent *= -1.0f;
                }
                return false;
            }
            fPrecedingLength += fPrevLen;
            if (fPrecedingLength <= fStation && fPrecedingLength + fLen >= fStation) {
                float fLocalStation = fStation - fPrecedingLength;
                const bool bInverse = thisSegDir == OrthogonalDirection::RIGHT || thisSegDir == OrthogonalDirection::TOP;
                if (bInverse)
                    fLocalStation = fLen - fLocalStation;
                retPoint = thisSeg.getPointAt(thisSegDir, fLocalStation, pTangent);
                if (bInverse && pTangent != nullptr) {
                    *pTangent *= -1.0f;
                }
                return false;
            }
            return true;
        });
    //float fPrecedingLength{ 0.0f };
    //glm::vec3 retPoint;
    //for (int i = 1; i < m_vPathSegments.size(); ++i) {
    //    const auto& [dir, segEnt] = m_vPathSegments.at(i);
    //    const auto& [prevDir, prevSegEnt] = m_vPathSegments.at(i - 1);
    //    const auto& prevSeg = entityManager.getComponent<PathSegmentComponent>(prevSegEnt);
    //    const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(segEnt);
    //    const auto oppositeDir = Tools::getOppositeDirection(dir);
    //    const float fPrevLen = prevSeg.m_pathSegment.length(oppositeDir);
    //    const float fLen = thisSeg.m_pathSegment.length(dir);
    //    if (fPrecedingLength <= fStation && fPrecedingLength + fPrevLen > fStation) {
    //        float fLocalStation = fStation - fPrecedingLength;
    //        const bool bInverse = oppositeDir == OrthogonalDirection::LEFT || oppositeDir == OrthogonalDirection::BOTTOM;
    //        if (bInverse) {
    //            fLocalStation = fPrevLen - fLocalStation;
    //        }
    //        retPoint = prevSeg.m_pathSegment.getPointAt(oppositeDir, fLocalStation, pTangent);
    //        if (bInverse && pTangent != nullptr) {
    //            *pTangent *= -1.0f;
    //        }
    //        break;
    //    }
    //    fPrecedingLength += fPrevLen;
    //    if (fPrecedingLength <= fStation && fPrecedingLength + fLen >= fStation) {
    //        float fLocalStation = fStation - fPrecedingLength;
    //        const bool bInverse = dir == OrthogonalDirection::RIGHT || dir == OrthogonalDirection::TOP;
    //        if (bInverse)
    //            fLocalStation = fLen - fLocalStation;
    //        retPoint = thisSeg.m_pathSegment.getPointAt(dir, fLocalStation, pTangent);
    //        if (bInverse && pTangent != nullptr) {
    //            *pTangent *= -1.0f;
    //        }
    //        break;
    //    }
    //    fPrecedingLength += fLen;
    //}
    return retPoint;
}

float Path::getStationAtSegment(Entity seg) const noexcept {

    float fRet{ -1.0f };
    traverseSegmentsPairwise([&fRet, seg](Entity prevSegEnt, const PathSegment& prevSeg, Entity thisSegEnt, const PathSegment& thisSeg, OrthogonalDirection prevSegDir, OrthogonalDirection thisSegDir, float fPrevLen, float fLen, float fPrecedingLength)
        {
            if (prevSegEnt == seg) {
                fRet = fPrecedingLength;
                return false;
            }
            else
            if (thisSegEnt == seg) {
                fPrecedingLength += fPrevLen + fLen;
                fRet = fPrecedingLength;
                return false;
            }
            return true;
        });

    return fRet;
}

Entity Path::getSegmentAtStation(float fStation) const
{
    if (fStation < 0.0f || fStation > length())
        throw std::runtime_error("Station must be between 0 and length!");
    
	Entity retEnt;
    traverseSegmentsPairwise([fStation, &retEnt](Entity prevSegEnt, const PathSegment& prevSeg, Entity thisSegEnt, const PathSegment& thisSeg, OrthogonalDirection prevSegDir, OrthogonalDirection thisSegDir, float fPrevLen, float fLen, float fPrecedingLength)
        {
            if (fPrecedingLength <= fStation && fPrecedingLength + fPrevLen > fStation) {
				retEnt = prevSegEnt;
                return false;
            }
            fPrecedingLength += fPrevLen;
            if (fPrecedingLength <= fStation && fPrecedingLength + fLen >= fStation) {
				retEnt = thisSegEnt;
                return false;
            }
            return true;
        });

	return retEnt;
}

void Path::traverseSegmentsPairwise(const std::function<bool(Entity, const PathSegment&, Entity, const PathSegment&, OrthogonalDirection, OrthogonalDirection, float, float, float)>& func) const noexcept {
    float fPrecedingLength{ 0.0f };
    for (int i = 1; i < m_vPathSegments.size(); ++i) {
        const auto& [dir, segEnt] = m_vPathSegments.at(i);
        const auto& [prevDir, prevSegEnt] = m_vPathSegments.at(i - 1);
        const auto& prevSeg = entityManager.getComponent<PathSegmentComponent>(prevSegEnt);
        const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(segEnt);
        const auto oppositeDir = Tools::getOppositeDirection(dir);
        const float fPrevLen = prevSeg.m_pathSegment.length(oppositeDir);
        const float fLen = thisSeg.m_pathSegment.length(dir);
        //if (fPrecedingLength <= fStation && fPrecedingLength + fPrevLen > fStation) {
        //    float fLocalStation = fStation - fPrecedingLength;
        //    const bool bInverse = oppositeDir == OrthogonalDirection::LEFT || oppositeDir == OrthogonalDirection::BOTTOM;
        //    if (bInverse) {
        //        fLocalStation = fPrevLen - fLocalStation;
        //    }
        //    retPoint = prevSeg.m_pathSegment.getPointAt(oppositeDir, fLocalStation, pTangent);
        //    if (bInverse && pTangent != nullptr) {
        //        *pTangent *= -1.0f;
        //    }
        //    break;
        //}
        const bool bContinue = func(prevSegEnt, prevSeg.m_pathSegment, segEnt, thisSeg.m_pathSegment, oppositeDir, dir, fPrevLen, fLen, fPrecedingLength);
        if (!bContinue)
            break;
        fPrecedingLength += fPrevLen;
        //if (fPrecedingLength <= fStation && fPrecedingLength + fLen >= fStation) {
        //    float fLocalStation = fStation - fPrecedingLength;
        //    const bool bInverse = dir == OrthogonalDirection::RIGHT || dir == OrthogonalDirection::TOP;
        //    if (bInverse)
        //        fLocalStation = fLen - fLocalStation;
        //    retPoint = thisSeg.m_pathSegment.getPointAt(dir, fLocalStation, pTangent);
        //    if (bInverse && pTangent != nullptr) {
        //        *pTangent *= -1.0f;
        //    }
        //    break;
        //}
        fPrecedingLength += fLen;
    }

}


void Path::setPathSegments(const decltype(m_vPathSegments)& vSegs) {
    if (vSegs.size() < 2) {
        throw std::runtime_error("Path must consist of at least 2 segments");
    }
//    const auto firstDir = vSegs.at(1).first; // first dir must be none, so we get the direction of the second segment

//    m_vPathSegments.clear();
//    // We define path segment to go from left to right and from bottom to top
//    // Therefore, if we're coming from the right or the top, the sequence must be reversed
//    if (firstDir == OrthogonalDirection::RIGHT || firstDir == OrthogonalDirection::TOP) {
//        for (auto riter = vSegs.crbegin(); riter != vSegs.crend(); ++riter) {
//            OrthogonalDirection dir;
//            if (riter == vSegs.crbegin())
//                dir = OrthogonalDirection::NONE;
//            else {
//                auto iter = std::prev(riter);
//                dir = Tools::getOppositeDirection(iter->first);
//            }
//            m_vPathSegments.emplace_back(std::make_pair(dir, riter->second));
//        }
//    } else {
//        m_vPathSegments = vSegs;
//    }
    m_vPathSegments = vSegs;
}

/*
 * \name append
 *
 * \date 30.11.2020
 * \param const Path& other: appends or prepends other to this path
 *
 * \description Joins two paths together. Paths must share a common path segment entity
 *              When the other path is added, the shared path segment entity is skipped and the
 *              connection direction (which used to be NONE) on the relevant path segment
 *              is adjusted.
 *
 */
void Path::append(const Path& other) {
    auto thisFirstSeg = m_vPathSegments.front().second; // .second is the entity which has the path segment component
    auto thisEndSeg = m_vPathSegments.back().second;
    auto otherFirstSeg = other.m_vPathSegments.front().second;
    auto otherEndSeg = other.m_vPathSegments.back().second;
    
    auto reverseSegments = [](const std::vector<std::pair<OrthogonalDirection, Entity>>& vSegs) {
        std::vector<std::pair<OrthogonalDirection, Entity>> vOutSegs;
        for (auto riter = vSegs.crbegin(); riter != vSegs.crend(); ++riter) {
            OrthogonalDirection dir;
            if (riter == vSegs.crbegin())
                dir = OrthogonalDirection::NONE;
            else {
                auto iter = std::prev(riter);
                dir = Tools::getOppositeDirection(iter->first);
            }
            vOutSegs.emplace_back(std::make_pair(dir, riter->second));
        }
        return vOutSegs;
    };

    if (thisFirstSeg == otherFirstSeg) {
        auto otherReversed = reverseSegments(other.m_vPathSegments);
        auto secondEntityOther = std::next(otherReversed.crbegin()); // skip shared entity and use next one to check connection
        const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(thisFirstSeg).m_pathSegment;
        const auto& otherSeg = entityManager.getComponent<PathSegmentComponent>(secondEntityOther->second).m_pathSegment;
        auto vConnDirs = thisSeg.connects(otherSeg);
        m_vPathSegments.front().first = vConnDirs.front();  // adjust connection direction 
        std::copy(secondEntityOther, otherReversed.crend(), m_vPathSegments.begin());
        return;
    }
    if (thisEndSeg == otherEndSeg) {
        auto otherReversed = reverseSegments(other.m_vPathSegments);
        auto secondEntityOther = std::next(otherReversed.cbegin());
        const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(thisEndSeg).m_pathSegment;
        const auto& otherSeg = entityManager.getComponent<PathSegmentComponent>(secondEntityOther->second).m_pathSegment;
        auto vConnDirs = thisSeg.connects(otherSeg);
        otherReversed.front().first = Tools::getOppositeDirection(vConnDirs.front());
        std::copy(secondEntityOther, otherReversed.cend(), std::back_inserter(m_vPathSegments));
        return;
    }
    if (thisFirstSeg == otherEndSeg) {
        auto secondEntityOther = std::next(other.m_vPathSegments.crbegin());
        const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(thisFirstSeg).m_pathSegment;
        const auto& otherSeg = entityManager.getComponent<PathSegmentComponent>(secondEntityOther->second).m_pathSegment;
        auto vConnDirs = thisSeg.connects(otherSeg);
        m_vPathSegments.front().first = vConnDirs.front();
        std::copy(secondEntityOther, other.m_vPathSegments.crend(), m_vPathSegments.begin());
        return;
    }
    if (thisEndSeg == otherFirstSeg) {
        auto otherTemp = other.m_vPathSegments;
        auto secondEntityOther = std::next(otherTemp.cbegin());
        const auto& thisSeg = entityManager.getComponent<PathSegmentComponent>(thisEndSeg).m_pathSegment;
        const auto& otherSeg = entityManager.getComponent<PathSegmentComponent>(secondEntityOther->second).m_pathSegment;
        auto vConnDirs = thisSeg.connects(otherSeg);
        otherTemp.front().first = Tools::getOppositeDirection(vConnDirs.front());
        std::copy(secondEntityOther, otherTemp.cend(), std::back_inserter(m_vPathSegments));
        return;
    }
}

}
