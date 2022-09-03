//
//  Model.h
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <functional>

namespace Ice {

struct Model {
    Model() noexcept = default;
	virtual ~Model() = default;
    
    MeshComponent* pMesh{ nullptr };
    RenderMaterialsComponent* pMaterials{ nullptr };

    bool operator==(const Model& other) const { return pMesh == other.pMesh; }
};

}

namespace std {
    template<>
    struct hash<Ice::Model> {
        size_t operator()(const Ice::Model& m) const {
            return std::hash<Ice::MeshComponent*>{}(m.pMesh);
        }
    };
}


//class Model {
//    using CoordType = float;
//
//protected:
//
//    std::string m_strCurrentMaterial;
//
//    std::vector<CoordType> m_vVertices;
//    std::vector<CoordType> m_vTexCoords;
//    std::vector<unsigned int> m_vVertexIndices;
//    std::vector<CoordType> m_vNormals;
//    std::vector<CoordType> m_vVerticesTransparent;
//    std::vector<CoordType> m_vTexCoordsTransparent;
//    std::vector<unsigned int> m_vVertexIndicesTransparent;
//    std::vector<CoordType> m_vNormalsTransparent;
//    std::string m_strName;
//    std::string m_strShader;
//    std::string m_strTextureAtlas;
//    glm::vec3 m_position;
//    glm::vec3 m_minBounds, m_maxBounds;
//    std::map<std::string, RenderMaterial> m_mMaterials;
//    std::map<std::string, std::vector<unsigned int>> m_mMaterialIndices;
//
//public:
//    Model();
//    Model(const Model&) = default;
//    Model& operator=(const Model&) = default;
//    virtual ~Model();
//
//    void setName(const std::string&);
//    void setCurrentMaterial(const std::string&) noexcept;
//    void setVertices(const std::vector<CoordType>&);
//    void setTexCoords(const std::vector<CoordType>&);
//    void setIndices(const std::vector<unsigned int>&);
//    void setMaterialIndices(const std::map<std::string, std::vector<unsigned int>>&) noexcept;
//    void setNormals(const std::vector<CoordType>&);
//    //void setTexture(std::unique_ptr<Texture>&&);
//    void setPosition(const glm::vec3&);
//    void setShaderId(const std::string&);
//    void setTextureAtlasId(const std::string&);
//    void setMaterials(const std::map<std::string, RenderMaterial>&) noexcept;
//
//    const auto& vertices() const noexcept { return m_vVertices; }
//    auto& vertices() noexcept { return m_vVertices; }
//    const auto& texCoords() const noexcept { return m_vTexCoords; }
//    auto& texCoords() noexcept { return m_vTexCoords; }
//    const auto& indices() const noexcept { return m_vVertexIndices; }
//    auto& indices() noexcept { return m_vVertexIndices; }
//    const auto& normals() const noexcept { return m_vNormals; }
//    auto& normals() noexcept { return m_vNormals; }
//
//    const auto& verticesTransparent() const noexcept { return m_vVerticesTransparent; }
//    auto& verticesTransparent() noexcept { return m_vVerticesTransparent; }
//    const auto& texCoordsTransparent() const noexcept { return m_vTexCoordsTransparent; }
//    auto& texCoordsTransparent() noexcept { return m_vTexCoordsTransparent; }
//    const auto& indicesTransparent() const noexcept { return m_vVertexIndicesTransparent; }
//    auto& indicesTransparent() noexcept { return m_vVertexIndicesTransparent; }
//    const auto& normalsTransparent() const noexcept { return m_vNormalsTransparent; }
//    auto& normalsTransparent() noexcept { return m_vNormalsTransparent; }
//
//    const auto& position() const noexcept { return m_position; }
//    const auto& shaderId() const noexcept { return m_strShader; }
//    const auto& textureAtlasId() const noexcept { return m_strTextureAtlas; }
//    const auto& currentMaterial() const noexcept { return m_strCurrentMaterial; }
//
//    const RenderMaterial& getMaterial(const std::string&) const;
//    const std::vector<unsigned int>& getMaterialIndices(const std::string&) const;
//    inline const auto& getMaterialIndices() const noexcept { return m_mMaterialIndices; }
//
//    inline glm::vec3& minBounds() noexcept { return m_minBounds; }
//    inline glm::vec3& maxBounds() noexcept { return m_maxBounds; }
//
//    void clearBuffers();
//};

#endif /* Model_h */
