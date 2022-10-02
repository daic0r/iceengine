//
//  ModelImporterCollada.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 12.10.20.
//  Copyright � 2020 Matthias Gruen. All rights reserved.
//

#include <Importers/ModelImporterCollada.h>
#include <filesystem>
#include <fstream>
#include <rapidxml/rapidxml.hpp>
#include <vector>
#include <sstream>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <Importers/packedVertex.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cstring>
#include <System/Tools.h>
#include <glm/gtx/string_cast.hpp>
#include <Components/AnimatedMeshComponent.h>
#include <Components/SkeletonComponent.h>
#include <Components/ModelAnimationComponent.h>
#include <Components/MeshComponent.h>

namespace Ice {

namespace fs = std::filesystem;

ModelImporterCollada::ModelImporterCollada(const std::string& strFile) {
    if (!fs::exists(strFile))
        throw std::runtime_error("File not found");
    m_strFile = strFile;
    m_correctiveTransform = glm::mat4{ glm::rotate(glm::mat4{1.0f}, glm::radians(-90.0f), glm::vec3{ 1.0f, 0.0f, 0.0f }) };
    m_invCorrectiveTransform = glm::inverse(m_correctiveTransform);
}

std::map<std::string, ModelImporterCollada::sSource> ModelImporterCollada::loadSources(rapidxml::xml_node<char>* pParentNode) {
    std::map<std::string, ModelImporterCollada::sSource> mRet;

    auto pSrcNode = pParentNode->first_node("source");
    while (pSrcNode != nullptr) {

        if (strcmp(pSrcNode->name(), "source") == 0) {

            std::string strSourceId = pSrcNode->first_attribute("id")->value();
            auto& srcRef = mRet[strSourceId];

            std::vector<float>& vTargetBuffer{ srcRef.vBuffer };
            std::vector<std::string>& vStringTargetBuffer{ srcRef.vStringBuffer };

            auto pFloatArrayNode = pSrcNode->first_node("float_array");
            auto pNameArrayNode = pSrcNode->first_node("Name_array");
            auto pDataArrayNode = pFloatArrayNode ? pFloatArrayNode : pNameArrayNode;

            // Fill accessor
            //////////////////////////////////////////////////////////////////////////
            auto pAccessor = pSrcNode->first_node("technique_common")->first_node("accessor");
            auto pParam = pAccessor->first_node("param");
            while (pParam != nullptr) {
                std::string strParamName = pParam->first_attribute("name")->value();
                std::string strParamType = pParam->first_attribute("type")->value();
                srcRef.accessor.mParams[strParamName] = strParamType;
                pParam = pParam->next_sibling();
            }

            srcRef.accessor.nCount = std::stoi(pAccessor->first_attribute("count")->value());
            srcRef.accessor.nStride = std::stoi(pAccessor->first_attribute("stride")->value());
            //////////////////////////////////////////////////////////////////////////

            std::stringstream ss{ pDataArrayNode->value() };
            if (pFloatArrayNode != nullptr) {
                float fData;
                while (ss >> fData) {
                    vTargetBuffer.emplace_back(fData);
                }
            }
            else {
                std::string strData;
                while (ss >> strData) {
                    vStringTargetBuffer.emplace_back(strData);
                }
            }
        }
        pSrcNode = pSrcNode->next_sibling();

    }

    return mRet;
}

void ModelImporterCollada::linkUpInputs(rapidxml::xml_node<char>* pParentNode, const std::map<std::string, ModelImporterCollada::sSource>& mSrcBuffers, std::map<int, std::string>& mOutOffset2Semantic, std::map<int, const ModelImporterCollada::sSource*>& mOutInputs) {
    auto pInput = pParentNode->first_node("input");
    if (pInput == nullptr)
        throw std::runtime_error("Node has no nested input node");
    while (pInput != nullptr && strcmp(pInput->name(), "input") == 0) {
        std::string strSemantic = pInput->first_attribute("semantic")->value();
        auto pOffsetAttr = pInput->first_attribute("offset");
        int nOffset = pOffsetAttr != nullptr ? std::stoi(pOffsetAttr->value()) : -1;
        if (nOffset == -1)
            throw std::runtime_error("Method can only be used for inputs with offsets");
        mOutOffset2Semantic[nOffset] = strSemantic;

        std::string strSrc;
        if (strSemantic == "VERTEX") {
            auto pVertices = pParentNode->parent()->first_node("vertices");
            if (pVertices == nullptr)
                throw std::runtime_error("Parent node has no vertices node");
            auto pPosition = pVertices->first_node("input");
            if (pPosition == nullptr)
                throw std::runtime_error("No input node found");
            if (strcmp(pPosition->first_attribute("semantic")->value(), "POSITION") == 0) {
                strSrc = pPosition->first_attribute("source")->value();
            }
        }
        else {
            strSrc = pInput->first_attribute("source")->value();
        }
        mOutInputs[nOffset] = &mSrcBuffers.at(strSrc.substr(1, strSrc.length() - 1));

        pInput = pInput->next_sibling();
    }
}

bool ModelImporterCollada::import(std::map<std::string, MeshComponent>& outMeshData, std::map<std::string, AnimatedMeshComponent>& outAnimatedMesh) noexcept {
    std::ifstream f{ m_strFile };
    if (!f)
        return false;

    std::vector<char> vBuffer{ std::istreambuf_iterator<char>{ f }, std::istreambuf_iterator<char>{} };
    f.close();
    vBuffer.emplace_back('\0');

    rapidxml::xml_document<> xml_doc;
    xml_doc.parse<0>(&vBuffer[0]);

    auto pRootNode = xml_doc.first_node("COLLADA");

    try {
        loadMaterials(pRootNode);
    }
    catch (const std::runtime_error& e) {
        std::cout << "ERROR: Could not load materials (" << e.what() << ")" << std::endl;
        return false;
    }
    try {
        loadJointData(pRootNode);
    }
    catch (const std::runtime_error& e) {
        std::cout << "ERROR: Could not load joint data (" << e.what() << ")" << std::endl;
        return false;
    }
    try {
        loadAnimations(pRootNode);
    }
    catch (const std::runtime_error& e) {
        std::cout << "ERROR: Could not load animations (" << e.what() << ")" << std::endl;
        return false;
    }
    
    rapidxml::xml_node<>* pLibGeomNode = pRootNode->first_node("library_geometries");
    if (pLibGeomNode == nullptr)
        return false;
    
    auto pGeometryNode = pLibGeomNode->first_node();
    std::map<std::string, sSource> mSources;

    // One geometry node for each object
    while (pGeometryNode != nullptr) {

        std::string strId = pGeometryNode->first_attribute("id")->value();
        std::string strObjName = pGeometryNode->first_attribute("name")->value();

        auto& meshDataAnimatedCurrentObject = outAnimatedMesh[strObjName];
        meshDataAnimatedCurrentObject.m_nNumJoints = m_vJoints.size();
        
        auto pMeshNode = pGeometryNode->first_node("mesh");

        // Parse source nodes
        //////////////////////////////////////////////////////////////////////////
        mSources = loadSources(pMeshNode);
        //////////////////////////////////////////////////////////////////////////

        // Look-up vector for already known combinations
        std::vector<packedVertex> vVertices;

        auto pPolyNode = pMeshNode->first_node("polylist");
        pPolyNode = pPolyNode ? pPolyNode : pMeshNode->first_node("triangles");
        std::string strVertexIndicesTagName{ pPolyNode->name() };
        while (pPolyNode != nullptr && std::string{ pPolyNode->name() } == strVertexIndicesTagName) {

            auto nVertexCount = std::stoi(pPolyNode->first_attribute("count")->value());
            std::string strCurrentMaterial{ pPolyNode->first_attribute("material")->value() };

            // Link up source buffers
            //////////////////////////////////////////////////////////////////////////
//            auto pInput = pPolyNode->first_node("input");
            std::map<int, std::string> mOffset2Semantic;
            std::map<int, const sSource*> mInputs;
            linkUpInputs(pPolyNode, mSources, mOffset2Semantic, mInputs);
            //////////////////////////////////////////////////////////////////////////

            auto pPrimitives = pPolyNode->first_node("p");
            if (pPrimitives == nullptr)
                return false;
            
            std::stringstream ss{ pPrimitives->value() };
            
            auto pVcountNode = pPolyNode->first_node("vcount");
            std::unique_ptr<std::stringstream> pSs_vcount;
            if (pVcountNode != nullptr) {
                pSs_vcount = std::make_unique<std::stringstream>(pVcountNode->value());
            }

            auto& meshDataCurrentObject = outMeshData[strObjName];
            auto& vIndicesCurMaterial = meshDataCurrentObject.materialIndices()[strCurrentMaterial];  //vOutIndices[strCurrentMaterial];

            bool bHasVertices = std::find_if(mOffset2Semantic.begin(), mOffset2Semantic.end(), [](const auto& kvp) { return kvp.second == "VERTEX" || kvp.second == "POSITION"; }) != mOffset2Semantic.end();
            bool bHasNormals = std::find_if(mOffset2Semantic.begin(), mOffset2Semantic.end(), [](const auto& kvp) { return kvp.second == "NORMAL"; }) != mOffset2Semantic.end();
            bool bHasTexCoord = std::find_if(mOffset2Semantic.begin(), mOffset2Semantic.end(), [](const auto& kvp) { return kvp.second == "TEXCOORD"; }) != mOffset2Semantic.end();
            
            packedVertex pv;
            int nIndex;
            for (int i = 0; i < nVertexCount; ++i) {
                int nNumVertsPerPrimitive;
                if (pSs_vcount)
                    *pSs_vcount >> nNumVertsPerPrimitive;
                else
                    nNumVertsPerPrimitive = 3;
                
                if (nNumVertsPerPrimitive != 3)
                    return false;
                
                for (int l = 0; l < nNumVertsPerPrimitive; ++l) {
                
                    glm::ivec4 jointIds;
                    glm::vec4 weights;

                    for (const auto& [j, pSource] : mInputs) {
                        ss >> nIndex;

                        //mTargetBuffers[j]->emplace_back(mInputs[j]->at(nIndex));
                        float* pPackedVertexTargetBuffer{ nullptr };
                        std::string strSemantic = mOffset2Semantic[j];
                        if (strSemantic == "VERTEX" || strSemantic == "POSITION") {
                            jointIds = glm::ivec4{};
                            weights = glm::vec4{};

                            pPackedVertexTargetBuffer = &pv.vertex[0];
                            
                            if (m_vVertexWeights.size() > 0) {
                                const auto& weightMap = m_vVertexWeights.at(nIndex);
                                int n{ 0 };
                                float fLen{};
                                for (auto riter = weightMap.rbegin(); riter != weightMap.rend(); ++riter) {
                                    jointIds[n] = static_cast<int>(riter->second);
                                    weights[n] = riter->first;
                                    fLen += (weights[n]*weights[n]);
                                    ++n;
                                    if (n > 3)
                                        break;
                                }
                                if (fLen > 0.0f) {
                                    weights = weights / sqrtf(fLen);
                                }
                            }
                        }
                        else if (strSemantic == "NORMAL")
                            pPackedVertexTargetBuffer = &pv.normal[0];
                        else if (strSemantic == "TEXCOORD")
                            pPackedVertexTargetBuffer = &pv.texCoord[0];
                        
                        // nullptr if we encounter an irrelevant semantic
                        if (pPackedVertexTargetBuffer == nullptr)
                            continue;

                        auto nStride = pSource->accessor.nStride;

                        // Does not yet support unnamed, ignored, params in an accessor which would have to be skipped
                        glm::vec4 tempVec{ 0.0f, 0.0f, 0.0f, 1.0f };
                        for (int k = 0; k < nStride; ++k) {
                            pPackedVertexTargetBuffer[k] = pSource->vBuffer[nStride * nIndex + k];
                        }
                    }

                    auto iter = std::find(vVertices.begin(), vVertices.end(), pv);
                    if (iter != vVertices.end()) {
                        vIndicesCurMaterial.emplace_back(std::distance(vVertices.begin(), iter));
                    }
                    else {
                        vVertices.emplace_back(pv);
                        if (bHasVertices) {
                            glm::vec4 vert{ pv.vertex[0], pv.vertex[1], pv.vertex[2], 1.0f };
                            vert = m_correctiveTransform * vert;
                            for (int i = 0; i < 3; ++i) {
                                if (vert[i] > meshDataCurrentObject.extents().maxPoint[i])
                                    meshDataCurrentObject.extents().maxPoint[i] = vert[i];
                                if (vert[i] < meshDataCurrentObject.extents().minPoint[i])
                                    meshDataCurrentObject.extents().minPoint[i] = vert[i];
                            }
                            meshDataCurrentObject.vertices().emplace_back(vert);
                        }
                        if (bHasTexCoord) {
                            meshDataCurrentObject.texCoords().emplace_back(pv.texCoord[0], pv.texCoord[1]);
                        }
                        if (bHasNormals) {
                            glm::vec4 normal{ pv.normal[0], pv.normal[1], pv.normal[2], 0.0f };
                            normal = m_correctiveTransform * normal;
                            meshDataCurrentObject.normals().emplace_back(normal);
                        }
                        if (m_vJoints.size() > 0) {
                            meshDataAnimatedCurrentObject.m_vJointIds.emplace_back(jointIds);
                            meshDataAnimatedCurrentObject.m_vWeights.emplace_back(weights);
                        }
                        vIndicesCurMaterial.emplace_back(vVertices.size() - 1);
                    }
                }
            }

            pPolyNode = pPolyNode->next_sibling();
        }

        pGeometryNode = pGeometryNode->next_sibling();
    }
    
    return true;
}

void ModelImporterCollada::loadImages(rapidxml::xml_node<char> *pRootNode) {
    auto pNodeLibImages = pRootNode->first_node("library_images");
    if (pNodeLibImages == nullptr)
        return;
    
    auto pNodeImage = pNodeLibImages->first_node("image");
    while (pNodeImage != nullptr && strcmp(pNodeImage->name(), "image") == 0) {
        auto pAttrId = pNodeImage->first_attribute("id");
        if (pAttrId == nullptr)
            throw std::runtime_error("image tag must have id attribute");
        auto strId = std::string{ pAttrId->value() };
        
        auto pNodeInitFrom = pNodeImage->first_node("init_from");
        if (pNodeInitFrom == nullptr)
            throw std::runtime_error("image tag must contain init_from tag");
        
        auto strFile = std::string{ pNodeInitFrom->value() };
        
        m_mImages.emplace(strId, strFile);
        
        pNodeImage = pNodeImage->next_sibling();
    }
    
}

void ModelImporterCollada::loadMaterials(rapidxml::xml_node<>* pRootNode) {
    try {
        loadImages(pRootNode);
    } catch (const std::runtime_error& e){
        std::cout << "Error loading images: " << e.what() << std::endl;
        return;
    }
    
    auto pNodeEffects = pRootNode->first_node("library_effects");
    if (pNodeEffects == nullptr)
        return;
        
    struct sMaterial {
        glm::vec3 ambient, diffuse, specular;
        std::string strTextureFile;
        float fAlpha;
        float fShininess;
        float fRefractionIndex;
    };

    std::map<std::string, sMaterial> mMatDefs;
    std::map<std::string, std::string> mSampler2Surface;
    std::map<std::string, std::string> mSurface2Image;
    
    // Read material definition from <library_effects>
    //////////////////////////////////////////////////////
    auto pNodeEffect = pNodeEffects->first_node("effect");
    while (pNodeEffect != nullptr && strcmp(pNodeEffect->name(), "effect") == 0) {
        std::string strEffectId{ pNodeEffect->first_attribute("id")->value() };
        auto& matRef = mMatDefs[strEffectId];

        auto pProfileCommonNode = pNodeEffect->first_node("profile_COMMON");
        if (pProfileCommonNode == nullptr)
            continue;
        
        // Read params for samplers and surfaces which are used to access the texture images
        //////////////////////////////////////////////////////
        auto pNewParamNode = pProfileCommonNode->first_node("newparam");
        while (pNewParamNode != nullptr && strcmp(pNewParamNode->name(), "newparam") == 0) {
            
            auto pSidAttrib = pNewParamNode->first_attribute("sid");
            if (pSidAttrib == nullptr)
                continue;
            
            auto strSid = std::string{ pSidAttrib->value() };
            
            auto pSampler2dNode = pNewParamNode->first_node("sampler2D");
            auto pSurfaceNode = pNewParamNode->first_node("surface");
            if (pSampler2dNode != nullptr) {
                
                auto pSourceNode = pSampler2dNode->first_node("source");
                if (pSourceNode == nullptr)
                    continue;
                
                auto strSurfaceSid = std::string{ pSourceNode->value() };
                
                mSampler2Surface.emplace(strSid, strSurfaceSid);
            } else
            if (pSurfaceNode != nullptr) {
                auto pInitFromNode = pSurfaceNode->first_node("init_from");
                if (pInitFromNode == nullptr)
                    continue;
                
                auto strImageSid = std::string{ pInitFromNode->value() };
                
                mSurface2Image.emplace(strSid, strImageSid);
            }
            
            pNewParamNode = pNewParamNode->next_sibling();
        }
        //////////////////////////////////////////////////////
        
        auto pTechniqueNode = pProfileCommonNode->first_node("technique");
        if (pTechniqueNode == nullptr)
            continue;
        auto pLightingModelNode = pTechniqueNode->first_node();
        if (pLightingModelNode == nullptr)
            continue;

        auto pMaterialProperty = pLightingModelNode->first_node();
        while (pMaterialProperty != nullptr) {
            glm::vec4 color;
            float fData;
            std::string strPropertyName{ pMaterialProperty->name() };

            if (strPropertyName == "emission" || strPropertyName == "ambient" || strPropertyName == "diffuse" || strPropertyName == "specular") {
                auto pColorNode = pMaterialProperty->first_node("color");
                
                std::string strTextureFile;
                if (pColorNode != nullptr) {
                    std::stringstream ss{ pColorNode->value() };
                    for (int i = 0; i < 4; ++i) {
                        ss >> color[i];
                    }
                } else
                if (auto pTextureNode = pMaterialProperty->first_node("texture"); pTextureNode != nullptr && strPropertyName == "diffuse") {
                    auto pTextureAttr = pTextureNode->first_attribute("texture");
                    if (pTextureAttr == nullptr)
                        continue;
                    
                    auto strSamplerSid = std::string{ pTextureAttr->value() };
                    strTextureFile = m_mImages.at(mSurface2Image.at(mSampler2Surface.at(strSamplerSid)));
                }

                if (strPropertyName == "emission" || strPropertyName == "ambient")
                    matRef.ambient = glm::vec3{ color[0], color[1], color[2] };
                else if (strPropertyName == "diffuse") {
                    if (pColorNode != nullptr) {
                        matRef.diffuse = glm::vec3{ color[0], color[1], color[2] };
                        matRef.fAlpha = color[3];
                    } else {
                        matRef.strTextureFile = strTextureFile;
                    }
                }
                else if (strPropertyName == "specular")
                    matRef.specular = glm::vec3{ color[0], color[1], color[2] };;
            }
            else if (strPropertyName == "shininess" || strPropertyName == "index_of_refraction" || strPropertyName == "transparency") {
                auto pFloatNode = pMaterialProperty->first_node("float");
                std::stringstream ss{ pFloatNode->value() };
                ss >> fData;

                if (strPropertyName == "shininess")
                    matRef.fShininess = fData;
                else if (strPropertyName == "index_of_refraction")
                    matRef.fRefractionIndex = fData;
                else if (strPropertyName == "transparency")
                    matRef.fAlpha = fData;
            }

            pMaterialProperty = pMaterialProperty->next_sibling();
        }

        pNodeEffect = pNodeEffect->next_sibling();
    }
    //////////////////////////////////////////////////////

    auto pNodeMaterials = pRootNode->first_node("library_materials");
    if (pNodeMaterials == nullptr)
        return;

    // Read material instances from <library_materials>
    //////////////////////////////////////////////////////
    auto pNodeMaterial = pNodeMaterials->first_node("material");
    while (pNodeMaterial != nullptr && strcmp(pNodeMaterial->name(), "material") == 0) {
        std::string strId{ pNodeMaterial->first_attribute("id")->value() };
        std::string strName{ pNodeMaterial->first_attribute("name")->value() };

        auto pNodeInstance = pNodeMaterial->first_node("instance_effect");
        if (pNodeInstance != nullptr) {

            std::string strEffectId{ pNodeInstance->first_attribute("url")->value() };

            const auto& refMaterialDef = mMatDefs.at(strEffectId.substr(1, strEffectId.length() - 1));

            auto& matInst = m_mMaterials[strId];
            matInst.setName(strName);
            matInst.setAmbient(refMaterialDef.ambient);
            matInst.setDiffuse(refMaterialDef.diffuse);
            matInst.setSpecular(refMaterialDef.specular);
            matInst.setAlpha(refMaterialDef.fAlpha);
            matInst.setSpecularExponent(refMaterialDef.fShininess);
            matInst.setRefractionIndex(refMaterialDef.fRefractionIndex);
            matInst.setTextureFile(refMaterialDef.strTextureFile);
        }

        pNodeMaterial = pNodeMaterial->next_sibling();
    }
    //////////////////////////////////////////////////////
}

void ModelImporterCollada::loadJointData(rapidxml::xml_node<char>* pRootNode) {
    auto pNodeControllers = pRootNode->first_node("library_controllers");
    if (pNodeControllers == nullptr)
        return;

    auto pNodeController = pNodeControllers->first_node("controller");
    if (pNodeController == nullptr)
        return;

    auto pNodeSkin = pNodeController->first_node("skin");
    if (pNodeSkin == nullptr)
        return;

    auto pNodeBindShapeMatrix = pNodeSkin->first_node("bind_shape_matrix");
    if (pNodeBindShapeMatrix != nullptr) {
        m_bindShapeMatrix = getMatrixFromNode(pNodeBindShapeMatrix);
    }
    
    auto mSources = loadSources(pNodeSkin);

    auto pNodeVertexWeights = pNodeSkin->first_node("vertex_weights");
    if (pNodeVertexWeights == nullptr)
        return;

    std::map<int, const sSource*> mInputs;
    std::map<int, std::string> mOffset2Semantic;

    linkUpInputs(pNodeVertexWeights, mSources, mOffset2Semantic, mInputs);

    auto nNumVertices = std::stoi(pNodeVertexWeights->first_attribute("count")->value());
    auto pNodeVCount = pNodeVertexWeights->first_node("vcount");
    auto pNodeV = pNodeVertexWeights->first_node("v");
    if (pNodeVCount == nullptr || pNodeV == nullptr)
        throw std::runtime_error("Could not find vcount or v node");


    int nWeightBufferOffset{ -1 };
    for (const auto& [nOffset, strSemantic] : mOffset2Semantic) {
        if (strSemantic == "WEIGHT") {
            nWeightBufferOffset = nOffset;
            break;
        }
    }
    if (nWeightBufferOffset == -1)
        throw std::runtime_error("Could not find weight semantic");

    std::stringstream ss_vcount{ pNodeVCount->value() };
    std::stringstream ss_v{ pNodeV->value() };
    for (int i = 0; i < nNumVertices; ++i) {
        size_t nNumJoints;
        ss_vcount >> nNumJoints;

        float fAccWeight{ 0.0f };
        std::map<float, size_t> mWeight2Joint; // <-- automatic ordering by weight
        for (int j = 0; j < nNumJoints; ++j) {
            size_t nJointIndex, nWeightIndex;
            ss_v >> nJointIndex >> nWeightIndex;
            float fWeight = mInputs[nWeightBufferOffset]->vBuffer.at(nWeightIndex);
            mWeight2Joint[fWeight] = nJointIndex;
            fAccWeight += fWeight;
        }
        assert(Tools::fEqual(fAccWeight, 1.0f));
        m_vVertexWeights.emplace_back(mWeight2Joint);
    }

    mOffset2Semantic.clear();
    mInputs.clear();

    auto pNodeJoints = pNodeSkin->first_node("joints");
    if (pNodeJoints == nullptr)
        throw std::runtime_error("Could not find joints node");

    auto pNodeJointsInputFirst = pNodeJoints->first_node("input");
    auto pNodeJointsInputSecond = pNodeJointsInputFirst->next_sibling();
    auto pNodeInputJoint = std::string{ pNodeJointsInputFirst->first_attribute("semantic")->value() } == "JOINT" ? pNodeJointsInputFirst : pNodeJointsInputSecond;
    auto pNodeInputInvBindMatrix = std::string{ pNodeJointsInputFirst->first_attribute("semantic")->value() } == "INV_BIND_MATRIX" ? pNodeJointsInputFirst : pNodeJointsInputSecond;

    std::string strSrcJoints{ pNodeInputJoint->first_attribute("source")->value() };
    std::string strSrcInvBindMatrices{ pNodeInputInvBindMatrix->first_attribute("source")->value() };

    const sSource& srcJoints = mSources.at(strSrcJoints.substr(1, strSrcJoints.length() - 1));
    const sSource& srcInvBindMatrices = mSources.at(strSrcInvBindMatrices.substr(1, strSrcInvBindMatrices.length() - 1));
    for (int i = 0; i < srcJoints.accessor.nCount; ++i) {
        sJoint joint;
        joint.nId = i;

        glm::mat4 invBindMatrix;
        for (int j = 0; j < srcInvBindMatrices.accessor.nStride; ++j) {
            // Column-major in glm, row-major in the file
            invBindMatrix[j % 4][j / 4] = srcInvBindMatrices.vBuffer.at(srcInvBindMatrices.accessor.nStride * i + j);
        }
        joint.invBindTransform = invBindMatrix * m_invCorrectiveTransform;
        // ^^^ right-multiply with the inverse
        // the vertices have already been corrected, and the root joint animation matrices have this transform applied as well
        // so that that the transforms happen in the correct space. So apply the inverse transform here so as to not
        // apply the transform to the vertices twice. It's all very confusing ;-)
        // Found in a comment under ThinMatrix's animation tutorial video:
        // https://www.youtube.com/watch?v=z0jb1OBw45I
        
        m_vJoints.emplace_back(joint);
        m_msId2Joint[srcJoints.vStringBuffer.at(i)] = m_vJoints.size() - 1;
    }

    loadJointHierarchy(pRootNode);
}

void ModelImporterCollada::loadJointHierarchy(rapidxml::xml_node<char>* pRootNode) {

    auto pNodeVisualScenes = pRootNode->first_node("library_visual_scenes");
    if (pNodeVisualScenes == nullptr)
        throw std::runtime_error("Node library_visual_scenes not found");

    auto pNodeVisualScene = pNodeVisualScenes->first_node("visual_scene");
    if (pNodeVisualScene == nullptr)
        throw std::runtime_error("Node visual_scene not found");

    rapidxml::xml_node<char>* pRootJointNode{ nullptr };
    auto pTempNode = pNodeVisualScene->first_node("node");

    while (pRootJointNode == nullptr) {
        if (pTempNode == nullptr)
            throw std::runtime_error("Node 'node' not found");

        if (std::string{ pTempNode->first_attribute("type")->value() } != "NODE")
            throw std::runtime_error("Top-level nodes must be of type NODE");

        // Get first actual joint node
        auto pSubNode = pTempNode->first_node("node");
        if (pSubNode != nullptr && strcmp(pSubNode->first_attribute("type")->value(), "JOINT") == 0) {
            pRootJointNode = pSubNode;
        }
        
        pTempNode = pTempNode->next_sibling();
    }

    traverseJointHierarchy(pRootJointNode);
}

/*!
* \brief
* [ModelImporterCollada::traverseJointHierarchy]
*
* \author matthias.gruen
* \date 2020/10/15
* [10/15/2020 matthias.gruen]
*/
void ModelImporterCollada::traverseJointHierarchy(rapidxml::xml_node<char>* pParentNode)
{
    std::string strId{ pParentNode->first_attribute("id")->value() };
    std::string strSid{ pParentNode->first_attribute("sid")->value() };
    std::string strName{ pParentNode->first_attribute("name")->value() };

    auto nJointIndex = m_msId2Joint.at(strSid);
    m_mId2Joint[strId] = nJointIndex;

    auto& joint = m_vJoints.at(nJointIndex);
    joint.strName = strName;
    
    joint.matrix = getTransformFromNode(pParentNode);
    glm::mat4 parentTransform{1.0f};
    if (joint.nParentIdx > -1) {
        parentTransform = m_vJoints.at(joint.nParentIdx).matrix;
    } else {
        auto pTopLevelNode = pParentNode->parent();
        parentTransform = getTransformFromNode(pTopLevelNode);
        m_invBindShapeMatrix = parentTransform;
        m_nRootJoint = nJointIndex;
    }
//        joint.matrix = parentTransform * joint.matrix;
//        std::cout << "Joint: " << joint.strName << ", vector: " << glm::to_string(joint.matrix * joint.invBindTransform * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f }) << std::endl;
    
    auto pChildNode = pParentNode->first_node("node");

    while (pChildNode != nullptr && strcmp(pChildNode->name(), "node") == 0) {
        if (strcmp(pChildNode->first_attribute("type")->value(), "JOINT") == 0) {

            std::string strChildSid{ pChildNode->first_attribute("sid")->value() };
            auto nChildJointIndex = m_msId2Joint.at(strChildSid);
            auto& childJoint = m_vJoints.at(nChildJointIndex);

            joint.vChildren.emplace_back(nChildJointIndex);
            childJoint.nParentIdx = static_cast<int>(nJointIndex);

            traverseJointHierarchy(pChildNode);
        }

        pChildNode = pChildNode->next_sibling();
    }
}

void ModelImporterCollada::loadAnimations(rapidxml::xml_node<char> *pRootNode) {
    auto pNodeAnimations = pRootNode->first_node("library_animations");
    if (pNodeAnimations == nullptr)
        return;
    
    auto pNodeContainer = pNodeAnimations->first_node("animation");
    if (pNodeContainer == nullptr)
        throw std::runtime_error("Animation node expected");
    
    auto pNodeAnimation = pNodeContainer->first_node("animation");
    if (pNodeAnimation == nullptr)
        pNodeAnimation = pNodeContainer;
    while (pNodeAnimation != nullptr && strcmp(pNodeAnimation->name(), "animation") == 0) {
        auto mSources = loadSources(pNodeAnimation);
        
        auto pNodeChannel = pNodeAnimation->first_node("channel");
        std::string strChannelTarget{ pNodeChannel->first_attribute("target")->value() };
        size_t nPos = strChannelTarget.find("/");
        std::string strTargetJointId = strChannelTarget.substr(0, nPos);
        auto jointIter = m_mId2Joint.find(strTargetJointId);
        
        // This will happen if we run into an IK joint which might not be part of the node hierarchy
        if (jointIter == m_mId2Joint.end()) {
            pNodeAnimation = pNodeAnimation->next_sibling();
            continue;
        }
        
        std::string strType = strChannelTarget.substr(nPos + 1, strChannelTarget.length() - nPos - 1);
        if (strType != "transform")
            throw std::runtime_error("Channel target not supported");
        std::string strChannelSource{ pNodeChannel->first_attribute("source")->value() };
        strChannelSource = stripLeadingChar(strChannelSource);
        
        rapidxml::xml_node<char>* pNodeSampler = nullptr;
        auto pNodeIterSampler = pNodeAnimation->first_node("sampler");
        while (pNodeIterSampler != nullptr && strcmp(pNodeIterSampler->name(), "sampler") == 0) {
            if (std::string{ pNodeIterSampler->first_attribute("id")->value() } == strChannelSource) {
                pNodeSampler = pNodeIterSampler;
                break;
            }
            pNodeIterSampler = pNodeIterSampler->next_sibling();
        }
        if (pNodeSampler == nullptr)
            throw std::runtime_error("sampler node that is linked to from channel not found under animation node");

        auto& joint = m_vJoints.at(jointIter->second);
        
        std::array<rapidxml::xml_node<char> *, 3> arInputs;
        auto pNodeInput = pNodeSampler->first_node("input");
        for (int i = 0; i < 3; ++i, pNodeInput = pNodeInput->next_sibling()) {
            if (pNodeInput == nullptr)
                throw std::runtime_error("Expected 3 inputs for the sampler");
            arInputs[i] = pNodeInput;
        }
        
        auto pInputSemanticInput = std::find_if(arInputs.begin(), arInputs.end(), [](auto pInput) {
            return strcmp(pInput->first_attribute("semantic")->value(), "INPUT") == 0;
        });
        auto pOutputSemanticInput = std::find_if(arInputs.begin(), arInputs.end(), [](auto pInput) {
            return strcmp(pInput->first_attribute("semantic")->value(), "OUTPUT") == 0;
        });
        if (pInputSemanticInput == arInputs.end() || pOutputSemanticInput == arInputs.end())
            throw std::runtime_error("INPUT or OUTPUT semantic not found");
        
        std::string strSrcInputSemantic{ (*pInputSemanticInput)->first_attribute("source")->value() };
        std::string strSrcOutputSemantic{ (*pOutputSemanticInput)->first_attribute("source")->value() };
        
        const auto& sourceInput = mSources.at(stripLeadingChar(strSrcInputSemantic));
        const auto& sourceOutput = mSources.at(stripLeadingChar(strSrcOutputSemantic));
        
        if (sourceInput.accessor.nStride != 1)
            throw std::runtime_error("Unexpected stride for source of INPUT semantic");
        if (sourceInput.accessor.mParams.size() != 1)
            throw std::runtime_error("Unexpected number of params for source of INPUT semantic");
        if (sourceInput.accessor.mParams.at("TIME") != "float")
            throw std::runtime_error("Unexpected parameter type for source of INPUT semantic");
        
        if (sourceOutput.accessor.mParams.size() != 1)
            throw std::runtime_error("Unexpected number of params for source of OUTPUT semantic");
        if (sourceOutput.accessor.mParams.at("TRANSFORM") != "float4x4")
            throw std::runtime_error("Unexpected parameter type for source of OUTPUT semantic (expected 4x4 matrix)");

        for (int i = 0; i < sourceInput.accessor.nCount; ++i) {
            float fTimeStamp = sourceInput.vBuffer.at(i);
            glm::mat4 matTransform;
            
            for (int j = 0; j < sourceOutput.accessor.nStride; ++j) {
                matTransform[j % 4][j / 4] = sourceOutput.vBuffer.at(sourceOutput.accessor.nStride * i + j);
            }
            
            // Apply corrective transform to root joint
            if (jointIter->second == m_nRootJoint) {
                matTransform = m_correctiveTransform * matTransform;
            }
            
            joint.mKeyframes[fTimeStamp] = matTransform;
        }

        pNodeAnimation = pNodeAnimation->next_sibling();
    }
}

std::string ModelImporterCollada::stripLeadingChar(const std::string& s) {
    return s.substr(1, s.length() - 1);
}

glm::mat4 ModelImporterCollada::getMatrixFromNode(rapidxml::xml_node<char>* pNode) {
    glm::mat4 ret;
    std::stringstream ss{ pNode->value() };
    for (int i = 0; i < 16; ++i) {
        float fData;
        ss >> fData;
        ret[i % 4][i / 4] = fData;
    }
    return ret;
}

glm::mat4 ModelImporterCollada::getTransformFromNode(rapidxml::xml_node<char> *pNode) {
    glm::mat4 ret{ 1.0f };
    if (auto pNodeMatrix = pNode->first_node("matrix"); pNodeMatrix != nullptr && strcmp(pNodeMatrix->first_attribute("sid")->value(), "transform") == 0) {
        ret = getMatrixFromNode(pNode->first_node("matrix"));
    } else {
        auto pTranslateNode = pNode->first_node("translate");

        glm::vec3 transl, scale;
        std::stringstream ss;
        if (pTranslateNode != nullptr) {
            ss = std::stringstream{ pTranslateNode->value() };
            for (int i = 0; i < 3; ++i)
                ss >> transl[i];
       }
        auto pRotateNode = pNode->first_node("rotate");
        float fRotateX{ 0.0f }, fRotateY{ 0.0f }, fRotateZ{ 0.0f };
        while (pRotateNode != nullptr && strcmp(pRotateNode->name(), "rotate") == 0) {
            ss = std::stringstream{ pRotateNode->value() };
            float fAngle;
            ss >> fAngle >> fAngle >> fAngle >> fAngle;
            std::string strSid{ pRotateNode->first_attribute("sid")->value() };
            if (strSid.find("X") != std::string::npos)
                fRotateX = fAngle;
            else
            if (strSid.find("Y") != std::string::npos)
                fRotateY = fAngle;
            else
            if (strSid.find("Z") != std::string::npos)
                fRotateZ = fAngle;
            pRotateNode = pRotateNode->next_sibling();
        }
        auto pScaleNode = pNode->first_node("scale");
        if (pScaleNode != nullptr) {
            ss = std::stringstream{ pScaleNode->value() };
            for (int i = 0; i < 3; ++i)
                ss >> scale[i];
            ret = glm::scale(ret, scale);
        }
        auto matScale = glm::scale(glm::mat4{ 1.0f }, scale);
        auto matRot = glm::rotate(glm::mat4{ 1.0f }, fRotateX, glm::vec3{ 1.0f, 0.0f, 0.0f });
        matRot = glm::rotate(matRot, fRotateY, glm::vec3{ 0.0f, 1.0f, 0.0f });
        matRot = glm::rotate(matRot, fRotateZ, glm::vec3{ 0.0f, 0.0f, 1.0f });
        auto matTranslate = glm::translate(glm::mat4{ 1.0f }, transl);
        ret = matTranslate * matRot * matScale;
    }
    
    return ret;
}

//void ModelImporterCollada::toAnimatedMeshComponent(AnimatedMeshComponent& ret) const noexcept {
//    ret.m_nNumJoints = m_vJoints.size();
//    
//    for (const auto& weightMap : m_vVertexWeights) {
//        int nIndex{ 0 };
//        glm::ivec4 v;
//        glm::vec4 w;
//        for (const auto& [jointId, weight] : weightMap) {
//            v[nIndex] = static_cast<int>(jointId);
//            w[nIndex] = weight;
//            ++nIndex;
//            if (nIndex > 3)
//                break;
//        }
//        w = glm::normalize(w);
//        ret.m_vJointIds.emplace_back(v);
//        ret.m_vWeights.emplace_back(w);
//    }
//}

void ModelImporterCollada::sJointToJoint(const sJoint& j, Joint& outJoint) const noexcept {
    outJoint = Joint{ static_cast<std::uint16_t>(j.nId), j.strName, j.invBindTransform };
    outJoint.setBindTransform(j.matrix);
    
    for (auto childJointIdx : j.vChildren) {
        Joint outChild;
        sJointToJoint(m_vJoints.at(childJointIdx), outChild);
        outJoint.addChild(outChild);
    }
}

void ModelImporterCollada::toSkeletonComponent(SkeletonComponent &ret) const noexcept {
    sJointToJoint(m_vJoints.at(m_nRootJoint), ret.m_rootJoint);
    
    ret.m_invBindShapeMatrix = m_invBindShapeMatrix;
}

void ModelImporterCollada::toModelAnimationComponent(ModelAnimationComponent& ret) const noexcept {
    ModelAnimation &ani = ret.animations["default"];
    
    float nLengthSeconds = std::numeric_limits<int>::min();
    
    for (int i = 0; i < m_vJoints.size(); ++i) {
        
        const auto& joint = m_vJoints.at(i);
        if (joint.mKeyframes.size() == 0)
            continue;
        
        // Get maximum frame time so we can determine the length of the animation
        const float nLastFrameTime = m_vJoints[i].mKeyframes.rbegin()->first;
        if (nLastFrameTime > nLengthSeconds)
            nLengthSeconds = nLastFrameTime;
        
        JointAnimation jointAnim;
        
        for (const auto& [fTimeStamp, transform] : joint.mKeyframes) {
            JointTransform jointTransform{ transform };
            jointAnim.jointTransforms()[fTimeStamp] = jointTransform;
        }
        
        ani.jointAnimations()[joint.nId] = jointAnim;
    }
    
    ani.setLengthSeconds(nLengthSeconds);
}

}
