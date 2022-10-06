//
//  MeshComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MeshComponent_h
#define MeshComponent_h

#include <IceEngine.h>
#include <vector>
#include <Components/ComponentIds.h>
#include <Entities/Entity.h>
#include <string>
#include <map>
#include <glm/vec3.hpp>
#include <System/Extents.h>
#include <Utils/serializables.h>

namespace Ice {

struct ICEENGINE_API MeshComponent {
private:
    std::vector<glm::vec3> m_vVertices;
    std::vector<glm::vec2> m_vTexCoords;
    std::vector<glm::vec4> m_vColors;
    std::vector<glm::vec3> m_vNormals;
    std::vector<unsigned int> m_vIndices;
    std::map<std::string, std::vector<unsigned int>> m_mMaterialIndices;
    std::string m_strShader;
    std::string m_strTextureAtlas;
    std::string m_strName;

public:
    Extents<glm::vec3> m_extents;
     
    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(name, m_strName)
    SERIALIZABLE_ATTRIB(vertices, m_vVertices)
    SERIALIZABLE_ATTRIB(uv, m_vTexCoords)
    //SERIALIZABLE_ATTRIB(colors, m_vColors)
    SERIALIZABLE_ATTRIB(normals, m_vNormals)
    SERIALIZABLE_ATTRIB(materialIndices, m_mMaterialIndices)
    SERIALIZABLE_ATTRIB(shader, m_strShader)
    SERIALIZABLE_ATTRIB(textureAtlas, m_strTextureAtlas)
    SERIALIZABLE_ATTRIB(extents, m_extents)
    END_DECLARE_SERIALIZABLE_ATTRIBS()

    std::string& textureAtlasId() noexcept { return m_strTextureAtlas; }
    std::string& shaderId() noexcept { return m_strShader; }
    auto& vertices() noexcept { return m_vVertices; }
    auto& texCoords() noexcept { return m_vTexCoords; }
    auto& colors() noexcept { return m_vColors; }
    std::vector<unsigned int>& indices() noexcept { return m_vIndices; }
    std::map<std::string, std::vector<unsigned int>>& materialIndices() noexcept { return m_mMaterialIndices; }
    auto& normals() noexcept { return m_vNormals; }
    Extents<glm::vec3>& extents() noexcept { return m_extents; }
    std::string& name() noexcept { return m_strName; }

    const std::string& textureAtlasId() const noexcept { return m_strTextureAtlas; }
    const std::string& shaderId() const noexcept { return m_strShader; }
    const auto& vertices() const noexcept { return m_vVertices; }
    const auto& texCoords() const noexcept { return m_vTexCoords; }
    const auto& colors() const noexcept { return m_vColors; }
    const std::vector<unsigned int>& indices() const noexcept { return m_vIndices; }
    const std::map<std::string, std::vector<unsigned int>>& materialIndices() const noexcept { return m_mMaterialIndices; }
    const auto& normals() const noexcept { return m_vNormals; }
    const Extents<glm::vec3>& extents() const noexcept { return m_extents; }
    const std::string& name() const noexcept { return m_strName; }
    
    static constexpr EntityComponentId id() noexcept { return MESH_COMPONENT; }

    friend ICEENGINE_API std::ostream& operator<<(std::ostream& os, const MeshComponent& c);
    friend ICEENGINE_API std::istream& operator>>(std::istream& is, MeshComponent& c);
};

ICEENGINE_API std::ostream& operator<<(std::ostream& os, const MeshComponent& c);
ICEENGINE_API std::istream& operator>>(std::istream& is, MeshComponent& c);

}

#endif /* MeshComponent_h */
