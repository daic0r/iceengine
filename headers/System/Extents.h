//
//  Extents.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Extents_h
#define Extents_h

#include <IceEngine.h>
#include <limits>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <Utils/serializables.h>

namespace Ice {

template<typename PointType>
struct ICEENGINE_API BaseExtents {};

template<>
struct ICEENGINE_API BaseExtents<glm::vec3> {
    glm::vec3 minPoint{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    glm::vec3 maxPoint{ std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(minPoint, minPoint)
    SERIALIZABLE_ATTRIB(maxPoint, maxPoint)
    END_DECLARE_SERIALIZABLE_ATTRIBS()

    friend void to_json(nlohmann::json& j, const BaseExtents& ext) {
        j["minPoint"] = ext.minPoint;
        j["maxPoint"] = ext.maxPoint;
    }

    friend void from_json(const nlohmann::json& j, BaseExtents& ext) {
        ext.minPoint = j.at("minPoint").template get<glm::vec3>();
        ext.maxPoint = j.at("maxPoint").template get<glm::vec3>();
    }
};

template<>
struct ICEENGINE_API BaseExtents<glm::vec2> {
    glm::vec2 minPoint{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
    glm::vec2 maxPoint{ std::numeric_limits<float>::min(), std::numeric_limits<float>::min() };
};

////////

template<typename PointType = glm::vec3>
struct ICEENGINE_API Extents : public BaseExtents<PointType> {
  
    bool contains(const Extents<PointType>& other) const {
        bool bContains{ true };
        for (int i = 0; i < other.length(); ++i) {
            if (other.minPoint[i] < BaseExtents<PointType>::minPoint[i] || other.maxPoint[i] > BaseExtents<PointType>::maxPoint[i]) {
                bContains = false;
                break;
            }
        }
        return bContains;
    }
    
    inline float dim(size_t nDim) const { return this->maxPoint[nDim] - this->minPoint[nDim]; }
};

using Extents2 = Extents<glm::vec2>;
using Extents3 = Extents<glm::vec3>;

}

#endif /* Extents_h */
