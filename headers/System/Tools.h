//
//  Tools.h
//  IceCraft
//
//  Created by Matthias Gruen on 26.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Tools_h
#define Tools_h

#include <IceEngine.h>
#include "Ray.h"
#include <glm/vec3.hpp>
#include "Types.h"
#include <glm/mat4x4.hpp>
#include <vector>
#include <string>
//#include <glm/detail/type_vec4.hpp>
//#include <glm/detail/type_vec3.hpp>
//#include <glm/detail/type_vec2.hpp>

namespace Ice {

class Frustum;

struct ICEENGINE_API Tools {
    static constexpr float EPSILON = .00001f;
    
    
//    static bool intersectAABB(const Ray& r, const glm::vec3& refPosition, const TBlockFaces& arFaces, int& x, int& y, int& z, float *fpDistance = nullptr);
//    static void getChunkCoords(const glm::vec3& pos, int& chunkX, int& chunkZ);
//    static void getChunkCoords(const glm::vec3& pos, int& chunkX, int& chunkZ, float& blockX, float& blockY, float& blockZ);
//    static glm::vec3 getWorldCoords(int chunkX, int chunkZ, float blockX, float blockY, float blockZ);
    static bool isVertexVisible(const glm::vec3& pos, const glm::mat4& mvp);
    static bool isVertexVisible(const Frustum& frust, const glm::vec3& pos);
    static bool checkMinMaxBounds(const Frustum& frust, const glm::vec3& posMin, const glm::vec3& posMax, bool bCheckMin);
	static bool checkMaxBounds(const Frustum& frust, const glm::vec3& pos);
    static glm::vec3 transformWithPerspectiveDivision(const glm::vec3& pos, const glm::mat4& mvp);
    static float sgn(float f);
    static bool fEqual(float a, float b);
    static glm::vec3 getLookAtVectorFromYawAndPitch(float yaw, float pitch) noexcept;
    static void printMatrix(const glm::mat4&) noexcept;
    static std::string vec3string(const glm::vec3&) noexcept;
    static std::string direction2string(OrthogonalDirection dir) noexcept;
    static OrthogonalDirection string2direction(const std::string& strDir);
    static std::string edgeOrientation2string(EdgeOrientation);
    static EdgeOrientation string2edgeOrientation(const std::string&);
    static OrthogonalDirection getOppositeDirection(OrthogonalDirection dir) noexcept;
    static OrthogonalDirection axis2direction(const glm::vec3&);
    static glm::vec3 direction2axis(OrthogonalDirection);
    static std::vector<std::string> tokenize(const std::string&, const std::string& strToken = " ");
	
    template<typename VecType>
	static bool vec_equals(const VecType& a, const VecType& b) noexcept {
		bool bRet{ true };
		for (typename VecType::length_type i = 0; i < VecType::length(); ++i) {
			bRet &= fEqual(a[i], b[i]);
			if (!bRet)
				break;
		}
		return bRet;
	}
    
    template <typename FloatType>
    static FloatType ceil0( const FloatType& value )
    {
       FloatType result = std::ceil( std::fabs( value ) );
       return (value < 0.0) ? -result : result;
    }
    
    template <typename FloatType>
    static FloatType floor0( const FloatType& value )
    {
       FloatType result = std::floor( std::fabs( value ) );
       return (value < 0.0) ? -result : result;
    }
};

}

#endif /* Tools_h */
