//
//  MeshComponent.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 05.11.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <System/StreamTools.h>

#include <Components/MeshComponent.h>

namespace Ice {

std::ostream& operator<<(std::ostream& os, const MeshComponent& c) {
    os << MeshComponent::id() << " ";
    
    StreamTools::writeStringToStream(os, c.name());
    os << " ";
    //std::copy(c.vertices().cbegin(), c.vertices().cend(), std::ostream_iterator<float>{ os, " " });
    os << c.vertices() << COMPONENT_FIELD_SEPARATOR;
    //std::copy(c.texCoords().cbegin(), c.texCoords().cend(), std::ostream_iterator<float>{ os, " " });
    os << c.texCoords() << COMPONENT_FIELD_SEPARATOR;
    //std::copy(c.normals().cbegin(), c.normals().cend(), std::ostream_iterator<float>{ os, " " });
    os << c.normals() << COMPONENT_FIELD_SEPARATOR;

//    for (const auto& [strMaterial, vIndices] : c.materialIndices()) {
//        os << strMaterial << " ";
//        std::copy(vIndices.cbegin(), vIndices.cend(), std::ostream_iterator<size_t>{ os, " " });
//        os << ";";
//    }
    
    os << c.materialIndices() << COMPONENT_FIELD_SEPARATOR;
    StreamTools::writeStringToStream(os, c.shaderId());
    os << " ";
    StreamTools::writeStringToStream(os, c.textureAtlasId());
    os << " ";
    os << c.m_extents.minPoint << " " << c.m_extents.maxPoint;
    
    return os;
}

std::istream& operator>>(std::istream& is, MeshComponent& c) {
    c.m_vVertices.clear();
    c.m_vTexCoords.clear();
    c.m_vNormals.clear();
    c.m_vIndices.clear();
    c.m_mMaterialIndices.clear();
    
    StreamTools::readStringFromStream(is, c.m_strName);
//    std::string strField;
//    std::getline(is, strField, COMPONENT_FIELD_SEPARATOR);
//    std::stringstream ss{ strField };
//    std::copy(std::istream_iterator<float>{ ss }, std::istream_iterator<float>{}, std::back_inserter(c.m_vVertices));
    is >> c.m_vVertices;
    
//    std::getline(is, strField, COMPONENT_FIELD_SEPARATOR);
//    ss = std::stringstream{ strField };
//    std::copy(std::istream_iterator<float>{ ss }, std::istream_iterator<float>{}, std::back_inserter(c.m_vTexCoords));
    is >> c.m_vTexCoords;
    
//    std::getline(is, strField, COMPONENT_FIELD_SEPARATOR);
//    ss = std::stringstream{ strField };
//    std::copy(std::istream_iterator<float>{ ss }, std::istream_iterator<float>{}, std::back_inserter(c.m_vNormals));
    is >> c.m_vNormals;
    
//    std::getline(is, strField, COMPONENT_FIELD_SEPARATOR);
//    size_t pos;
//    while ((pos = strField.find(COMPONENT_FIELD_SEPARATOR)) != std::string::npos) {
//        std::string strMaterialIndices;
//        std::string strMaterial;
//        strMaterialIndices = strField.substr(0, pos-1);
//        std::stringstream ss{ strMaterialIndices };
//        ss >> strMaterial;
//
//        std::copy(std::istream_iterator<size_t>{ ss }, std::istream_iterator<size_t>{}, std::back_inserter(c.m_mMaterialIndices[strMaterial]));
//
//        strField = strField.substr(pos + 1);
//    }
//
    is >> c.m_mMaterialIndices;
    
    StreamTools::readStringFromStream(is, c.m_strShader);
    StreamTools::readStringFromStream(is, c.m_strTextureAtlas);
    is >> c.m_extents.minPoint >> c.m_extents.maxPoint;

    return is;
}

}
