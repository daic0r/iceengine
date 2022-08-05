//
//  RenderMaterialsComponent.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Components/RenderMaterialsComponent.h>
#include <System/RenderMaterial.h>
#include <stdexcept>
#include <System/StreamTools.h>
#include <System/Config.h>
#include <System/SystemServices.h>

namespace Ice {

const RenderMaterial& RenderMaterialsComponent::getMaterial(const std::string& strMat) const {
    if (m_mMaterials.find(strMat) == m_mMaterials.end()) {
        auto iter = std::find_if(m_mMaterials.begin(), m_mMaterials.end(), [&strMat](const auto& kvp) {
            return kvp.second.name() == strMat;
        });
        
        if (iter == m_mMaterials.end())
            throw std::runtime_error("Material not found");
        
        return iter->second;
    }
    return m_mMaterials.at(strMat);
}

std::ostream& operator<<(std::ostream& os, const RenderMaterialsComponent& r) {
    os << RenderMaterialsComponent::id() << " ";
    os << r.m_mMaterials;
    return os;
}

std::istream& operator>>(std::istream& is, RenderMaterialsComponent& r) {
	r.materials().clear();
    is >> r.m_mMaterials;
    for (auto& [strName, mat] : r.m_mMaterials) {
        if (mat.textureFile().length() > 0) {
            mat.setTextureFile(systemServices.getConfig()->prependAssetPath("Models/" + mat.textureFile()));
        }
    }
    return is;
}

}
