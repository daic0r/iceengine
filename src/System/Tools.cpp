//
//  Tools.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 26.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Tools.h>
#include <glm/vec2.hpp>
#include <System/Config.h>
#include <math.h>
#include <Renderer/Frustum.h>
#include <vector>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <algorithm>

namespace Ice {

//bool Tools::intersectAABB(const Ray& r, const glm::vec3& refPosition, const TBlockFaces& arFaces, int& x, int& y, int& z, float *fpDistance) {
////auto checkIntersection = [&r, this](const TBlockFaces& arFaces, int& x, int& y, int& z) {
//
//    auto faces = arFaces;
//
//    std::tuple<float, glm::vec3, int> foundPointAtDistance{ std::numeric_limits<float>::max(), glm::vec3{}, -1};
//    int nNumFound { 0 };
//    bool bFound = false;
//
//    // Check all 6 faces
//    for (int i = 0; i < 6; ++i) {
//        auto nTargetDimension = i / 2; // x, y or z
//        auto& face = faces[i];
//        const float fTarget = face[nTargetDimension]; // get dimension value (they should all be equal for the face)
//        float t = (fTarget - r.origin()[nTargetDimension]) / r.direction()[nTargetDimension];
//        // no intersection
//        if (t <= 0.0f)
//            continue;
//
//        // get 2D vectors of the remaining 2 dimensions for bounds check
//        glm::vec2 hitPoint, faceMin, faceMax;
//        int nIndex = 0;
//        for (int j = 0; j < 3; ++j) {
//            if (j == nTargetDimension)
//                continue;
//            hitPoint[nIndex] = r.origin()[j] + r.direction()[j] * t;
//            faceMin[nIndex] = face[j];
//            faceMax[nIndex] = face[6 + j];    // 3rd vertex (x, y and z)
//            if (faceMin[nIndex] > faceMax[nIndex])
//                std::swap(faceMin[nIndex], faceMax[nIndex]);
//            ++nIndex;
//        }
//
//        if (hitPoint.x >= faceMin.x && hitPoint.x <= faceMax.x && hitPoint.y >= faceMin.y && hitPoint.y <= faceMax.y) {
//            if (t < std::get<0>(foundPointAtDistance)) {
//                glm::vec3 intersectAt = r.origin() + r.direction() * t;
//                foundPointAtDistance = std::make_tuple(t, intersectAt, i);
//                bFound = true;
//                ++nNumFound;
//                if (nNumFound == 2) // max 2 intersections possible
//                    break;
//            }
//        }
//    }
//
//    if (bFound) {
//        // deliberate narrowing, we only want the integer part
//        x = std::get<1>(foundPointAtDistance).x - refPosition.x;
//        y = std::get<1>(foundPointAtDistance).y - refPosition.y;
//        z = std::get<1>(foundPointAtDistance).z - refPosition.z;
//        switch (std::get<2>(foundPointAtDistance)) {
//            case 1:
//                x -= 1;
//                break;
//            case 3:
//                y -= 1;
//                break;
//            case 5:
//                z += 1;
//                break;
//        }
//    }
//
//    if (fpDistance != nullptr) {
//        *fpDistance = std::get<0>(foundPointAtDistance);
//    }
//
//    return bFound;
//}

//void Tools::getChunkCoords(const glm::vec3 &pos, int &chunkX, int &chunkZ, float &blockX, float& blockY, float &blockZ) {
//    getChunkCoords(pos, chunkX, chunkZ);
//    blockX = fmod(pos.x, Config::CHUNK_DIM);
//    blockZ = fmod(-pos.z, Config::CHUNK_DIM);
//    blockY = pos.y;
//}
//
//void Tools::getChunkCoords(const glm::vec3 &pos, int &chunkX, int &chunkZ) {
//    chunkX = pos.x / Config::CHUNK_DIM;
//    chunkZ = -pos.z / Config::CHUNK_DIM;
//}

//glm::vec3 Tools::getWorldCoords(int chunkX, int chunkZ, float blockX, float blockY, float blockZ) {
//    glm::vec3 retVec;
//    retVec.x = chunkX * Config::CHUNK_DIM + blockX;
//    retVec.y = blockY;
//    retVec.z = - (chunkZ * Config::CHUNK_DIM + blockZ);
//    return retVec;
//}

bool Tools::isVertexVisible(const glm::vec3& pos, const glm::mat4& mvp) {
    auto temp = transformWithPerspectiveDivision(pos, mvp);
    return temp.x >= -1.0f-EPSILON && temp.x <= 1.0f+EPSILON /* && temp.y >= -1.0f && temp.y <= 1.0f */ && temp.z >= -1.0f-EPSILON && temp.z <= 1.0f+EPSILON;
}

/*!
* \brief
* [Tools::isVertexVisible]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
bool Tools::isVertexVisible(const Frustum& frust, const glm::vec3& pos)
{
    std::unordered_map<FaceDirection, float> mDists;
    mDists[eFront] = frust.dist(eFront, pos);
	mDists[eBack] = frust.dist(eBack, pos);
	mDists[eLeft] = frust.dist(eLeft, pos);
	mDists[eRight] = frust.dist(eRight, pos);
	//mDists[eBottom] = frust.dist(eBottom, pos);
	//mDists[eTop] = frust.dist(eTop, pos);
    for (const auto& kvp : mDists) {
        if (sgn(kvp.second) < -EPSILON)
            return false;
    }
    return true;
}

/*!
* \brief
* [Tools::checkMinBounds]
*
* bCheckMin: true = test minimum bounds point (n-vertex), false = check maximum bounds point (p-vertex)
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
bool Tools::checkMinMaxBounds(const Frustum& frust, const glm::vec3& posMin, const glm::vec3& posMax, bool bCheckMin)
{
    std::vector<FaceDirection> vDirs;

    if (bCheckMin) {
        vDirs.emplace_back(eFront);
        vDirs.emplace_back(eRight);
        vDirs.emplace_back(eTop);
    } else {
        vDirs.emplace_back(eBack);
        vDirs.emplace_back(eLeft);
        vDirs.emplace_back(eBottom);
    }
    for (auto dir : vDirs) {
        auto bounds = bCheckMin ? posMax : posMin;
        const auto& normal = frust.planeNormal(dir);
        
        // explanation: for the max bounds check for example, the box begins to be on the visible side of the plane
        // when the max point (i.e. the side facing the same direction as the plane's normal) is on its positive side
        // (i.e. the normal is positive). for example, if the max point is to the right of the left plane, the box begins
        // to be visible. for the min bounds check, the min point is closer to the right plane, so we're using the min point here.
        // once the min point is to the left of the right plane, the box begins to be visible. this is done for each dimension.
        for (int i = 0; i < 3; ++i) {
            if (normal[i] >= 0.0f)
                bounds[i] = bCheckMin ? posMin[i] : posMax[i];
        }
        const float fDist = frust.dist(dir, bounds);
        if (fDist < 0.0f)
            return false;
    }

    return true;
}

/*!
* \brief
* [Tools::checkMaxBounds]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
bool Tools::checkMaxBounds(const Frustum& frust, const glm::vec3& pos)
{
	std::unordered_map<FaceDirection, float> mDists;
	mDists[eBack] = frust.dist(eBack, pos);
	mDists[eLeft] = frust.dist(eLeft, pos);
	mDists[eBottom] = frust.dist(eBottom, pos);
	for (const auto& kvp : mDists) {
		if (sgn(kvp.second) < -EPSILON)
			return false;
	}
	return true;
}

glm::vec3 Tools::transformWithPerspectiveDivision(const glm::vec3& pos, const glm::mat4& mvp) {
    glm::vec4 temp{ pos, 1.0f };
    temp = mvp * temp;
    temp /= temp.w;
    return temp;
}

/*!
* \brief
* [Tools::sgn]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
float Tools::sgn(float f)
{
    if (fEqual(f, 0.0f))
        return 0.0f;
    else
    if (f < 0.0f)
        return -1.0f;

    return 1.0f;
}

/*!
* \brief
* [Tools::fEqual]
*
* \author matthias.gruen
* \date 2020/08/31
* [8/31/2020 matthias.gruen]
*/
bool Tools::fEqual(float a, float b)
{
    float subtr = a - b;
    return abs(subtr) <= EPSILON;
}

glm::vec3 Tools::getLookAtVectorFromYawAndPitch(float yaw, float pitch) noexcept {
    glm::vec3 lookAt;
    lookAt.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    lookAt.y = sin(glm::radians(pitch));
    lookAt.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return lookAt;
}

void Tools::printMatrix(const glm::mat4 &mat) noexcept {
    for (int row = 0; row < 4; ++row) {
        std::cout << "[ ";
        for (int col = 0; col < 4; ++col) {
            std::cout << mat[col][row];
            if (col < 3)
                std::cout << "\t";
        }
        std::cout << " ]\n";
    }
    std::cout << std::endl;
}

std::string Tools::vec3string(const glm::vec3& v) noexcept {
    std::stringstream ss;
    ss << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return ss.str();
}

std::string Tools::direction2string(OrthogonalDirection dir) noexcept {
    std::string strRet;
    switch (dir) {
        case OrthogonalDirection::LEFT:
            strRet = "left";
            break;
        case OrthogonalDirection::RIGHT:
            strRet = "right";
            break;
        case OrthogonalDirection::TOP:
            strRet = "top";
            break;
        case OrthogonalDirection::BOTTOM:
            strRet = "bottom";
            break;
    }
    return strRet;
}

OrthogonalDirection Tools::string2direction(const std::string& strDir) {
    std::string s;
    std::transform(strDir.cbegin(), strDir.cend(), std::back_inserter(s), ::tolower);
    if (s == "left")
        return OrthogonalDirection::LEFT;
    else
    if (s == "right")
        return OrthogonalDirection::RIGHT;
    else
    if (s == "top")
        return OrthogonalDirection::TOP;
    else
    if (s == "bottom")
        return OrthogonalDirection::BOTTOM;
    
    throw std::runtime_error("Invalid direction string");
}

OrthogonalDirection Tools::getOppositeDirection(OrthogonalDirection dir) noexcept {
    switch (dir) {
        case OrthogonalDirection::LEFT:
            return OrthogonalDirection::RIGHT;
        case OrthogonalDirection::RIGHT:
            return OrthogonalDirection::LEFT;
        case OrthogonalDirection::TOP:
            return OrthogonalDirection::BOTTOM;
        case OrthogonalDirection::BOTTOM:
            return OrthogonalDirection::TOP;
        default:
            return OrthogonalDirection::NONE;
    }
}

OrthogonalDirection Tools::axis2direction(const glm::vec3& xAxis) {
    OrthogonalDirection dir = OrthogonalDirection::NONE;
    if (Tools::fEqual(xAxis.x, 1.0f)) {
        dir = OrthogonalDirection::RIGHT;
    }
    else if (Tools::fEqual(xAxis.x, -1.0f)) {
        dir = OrthogonalDirection::LEFT;
    }
    else if (Tools::fEqual(xAxis.z, 1.0f)) {
        dir = OrthogonalDirection::BOTTOM;
    }
    else if (Tools::fEqual(xAxis.z, -1.0f)) {
        dir = OrthogonalDirection::TOP;
    }
    return dir;
}

glm::vec3 Tools::direction2axis(OrthogonalDirection dir) {
    switch (dir) {
        case OrthogonalDirection::RIGHT:
            return glm::vec3{ 1.0f, 0.0f, 0.0f };
        case OrthogonalDirection::LEFT:
            return glm::vec3{ -1.0f, 0.0f, 0.0f };
        case OrthogonalDirection::BOTTOM:
            return glm::vec3{ 0.0f, 0.0f, 1.0f };
        case OrthogonalDirection::TOP:
            return glm::vec3{ 0.0f, 0.0f, -1.0f };
        default:
            return glm::vec3{};
    }
}

EdgeOrientation Tools::string2edgeOrientation(const std::string& str) {
    std::string s;
    std::transform(str.cbegin(), str.cend(), std::back_inserter(s), ::tolower);
    if (s == "withpath")
        return EdgeOrientation::WITH_PATH;
    else
    if (s == "againstpath")
        return EdgeOrientation::AGAINST_PATH;
    else
    if (s == "both")
        return EdgeOrientation::BOTH;
    
    throw std::runtime_error("Invalid orientation string");
}

std::string Tools::edgeOrientation2string(EdgeOrientation o) {
    switch (o) {
        case EdgeOrientation::WITH_PATH:
            return "withpath";
        case EdgeOrientation::AGAINST_PATH:
            return "againstpath";
        case EdgeOrientation::BOTH:
            return "both";
    }
}

std::vector<std::string> Tools::tokenize(const std::string& strString, const std::string& strToken) {
    size_t pos;
    size_t lastPos{ 0 };
    std::vector<std::string> vRet;
    while ((pos = strString.find(strToken, lastPos)) != std::string::npos) {
        std::string strSub = strString.substr(lastPos, pos - lastPos);
        vRet.push_back(strSub);
        lastPos += strSub.length() + strToken.length();
    }
    vRet.push_back(strString.substr(lastPos));
    return vRet;
}

}
