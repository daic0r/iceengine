//
//  Model.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 08.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//


#include <System/Model.h>
//#include <SDL2/SDL.h>
//#include "Config.h"
//#include "Chunk.h"
//#include <unordered_map>
//#include <iostream>
//#include "BlockLibrary.h"
//#include "Controller.h"
//#include <glm/gtc/matrix_transform.hpp>

/////////////
// ONLY DONE HERE FOR DEBUG PURPOSES
//#include "TextureManager.h"
/////////////

namespace Ice {

//Model::Model() {
//    for (int i = 0; i < 3; ++i) {
//        m_minBounds[i] = std::numeric_limits<float>::max();
//        m_maxBounds[i] = std::numeric_limits<float>::min();
//    }
//    setShaderId("Model");
//}
//
//Model::~Model() {
//
//}
//
//void Model::setName(const std::string& name) {
//    m_strName = name;
//}
//
//void Model::setVertices(const std::vector<float>& vertices) {
//    m_vVertices = vertices;
//}
//
//void Model::setTexCoords(const std::vector<float>& texCoords) {
//    m_vTexCoords = texCoords;
//}
//
////void Model::setIndices(const std::vector<unsigned int>& indices) {
////    m_vVertexIndices = indices;
////}
//
//void Model::setNormals(const std::vector<CoordType>& normals) {
//    m_vNormals = normals;
//}
//
////void Model::setTexture(std::unique_ptr<Texture>&& pTex) {
//
//void Model::setPosition(const glm::vec3& position) {
//    m_position = position;
//}
//
//void Model::setShaderId(const std::string& strId) {
//    m_strShader = strId;
//}
//
//void Model::setTextureAtlasId(const std::string& strId) {
//    m_strTextureAtlas = strId;
//}
//
//void Model::setCurrentMaterial(const std::string& strMaterial) noexcept {
//    m_strCurrentMaterial = strMaterial;
//}
//
//void Model::setMaterials(const std::map<std::string, RenderMaterial>& mMaterials) noexcept {
//    m_mMaterials = mMaterials;
//}
//
//void Model::setMaterialIndices(const std::map<std::string, std::vector<unsigned int>>& mIndices) noexcept {
//    m_mMaterialIndices = mIndices;
//}
//
//
//void Model::clearBuffers() {
//    vertices().clear();
//    texCoords().clear();
//    indices().clear();
//    normals().clear();
//    verticesTransparent().clear();
//    texCoordsTransparent().clear();
//    indicesTransparent().clear();
//    normalsTransparent().clear();
//}
//
//const RenderMaterial& Model::getMaterial(const std::string& strMat) const {
//    if (m_mMaterials.find(strMat) == m_mMaterials.end())
//        throw std::runtime_error("Material not found");
//    return m_mMaterials.at(strMat);
//}
//
//const std::vector<unsigned int>& Model::getMaterialIndices(const std::string& strMat) const {
//    if (m_mMaterialIndices.find(strMat) == m_mMaterialIndices.end())
//        throw std::runtime_error("Material not found");
//    return m_mMaterialIndices.at(strMat);
//}

}
