#include <Importers/ModelImporterGlTF.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <filesystem>
#include <iostream>
#include <vector>
#include <glm/vec3.hpp>
#include <ranges>
#include <memory>
#include <limits>

namespace Ice
{
    ModelImporterGlTF::ModelImporterGlTF(std::string_view strFile) : m_strFile{ strFile }
    {
        if (!std::filesystem::exists(strFile)) {
            throw std::runtime_error("File doesn't exist");
        }
    }

    bool ModelImporterGlTF::import() {
        tinygltf::TinyGLTF gltf_ctx;

        tinygltf::Model model;
        std::string strErr, strWarn;
        if (!gltf_ctx.LoadASCIIFromFile(&model, &strErr, &strWarn, m_strFile)) {
            throw std::runtime_error("glTF couldn't be loaded");
        }

        // Materials
        std::vector<RenderMaterial> vMaterials;
        vMaterials.reserve(model.materials.size());

        for (const auto& material : model.materials) {
            RenderMaterial mat;
            mat.setDiffuse(glm::vec3{ material.pbrMetallicRoughness.baseColorFactor[0], material.pbrMetallicRoughness.baseColorFactor[1], material.pbrMetallicRoughness.baseColorFactor[2] });
            mat.setAmbient(mat.diffuse());
            mat.setName(material.name);
            vMaterials.push_back(mat);
            m_mMaterials.emplace(material.name, mat);
        }
        ///////////////////////////////////////////////////////////////////////

        auto makeTypedBufferFromBufferView = []<typename T>(const tinygltf::Model& model, const tinygltf::BufferView& view) {
            std::vector<T> vRet;
            vRet.resize(view.byteLength / sizeof(T));
            std::memcpy(&vRet[0], &model.buffers.at(view.buffer).data[view.byteOffset], view.byteLength);
            return vRet;
        };

        for (const auto& mesh : model.meshes) {
            auto& mCurObject = m_mMeshes[mesh.name];
            auto& mCurAniObject = m_mAniMeshes[mesh.name];

            std::size_t nPrimIndex{};

            glm::vec3 minVertex{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
            glm::vec3 maxVertex{ -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };

            for (const auto& prim : mesh.primitives) { // material groups
                const auto nStartIndex = mCurObject.vertices().size();

                for (const auto& [strAttrName, nAccessorIndex] : prim.attributes) { // vertex attributes for each group
                    const auto& accessor = model.accessors.at(nAccessorIndex);

                    const auto& view = model.bufferViews.at(accessor.bufferView);

                    if (strAttrName == "POSITION") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                        assert(accessor.type == TINYGLTF_TYPE_VEC3);

                        const auto buf = makeTypedBufferFromBufferView.template operator()<glm::vec3>(model, view);
                        std::ranges::transform(buf, std::back_inserter(mCurObject.vertices()), [&minVertex,&maxVertex](const glm::vec3& v) {
                            for (glm::length_t i{}; i < 3; ++i) {
                                if (v[i] < minVertex[i])
                                    minVertex[i] = v[i];
                                if (v[i] > maxVertex[i])
                                    maxVertex[i] = v[i];
                            }
                            return v;
                        });
                    }
                    else
                    if (strAttrName == "NORMAL") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                        assert(accessor.type == TINYGLTF_TYPE_VEC3);

                        const auto buf = makeTypedBufferFromBufferView.template operator()<glm::vec3>(model, view);
                        mCurObject.normals().insert(mCurObject.normals().end(), buf.begin(), buf.end());
                    }
                    else
                    if (strAttrName == "JOINTS_0") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE);
                        assert(accessor.type == TINYGLTF_TYPE_VEC4);
                        
                        const auto buf = makeTypedBufferFromBufferView.template operator()<glm::i8vec4>(model, view);
                        std::ranges::transform(buf, std::back_inserter(mCurAniObject.m_vJointIds), [](const glm::i8vec4& elem) {
                            return glm::ivec4{ elem.x, elem.y, elem.z, elem.w };
                        });
                    }
                    else
                    if (strAttrName == "WEIGHTS_0") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                        assert(accessor.type == TINYGLTF_TYPE_VEC4);

                        const auto buf = makeTypedBufferFromBufferView.template operator()<glm::vec4>(model, view);
                        mCurAniObject.m_vWeights.insert(mCurAniObject.m_vWeights.end(), buf.begin(), buf.end());
                    }
                } // attributes

                mCurObject.extents().minPoint = minVertex;
                mCurObject.extents().maxPoint = maxVertex;

                const auto& indexAccessor = model.accessors.at(prim.indices);
                assert(indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);
                const auto& indexBufferView = model.bufferViews.at(indexAccessor.bufferView);
                const auto buf = makeTypedBufferFromBufferView.template operator()<unsigned short>(model, indexBufferView);
                auto& vMaterialIndices = mCurObject.materialIndices()[vMaterials.at(prim.material).name()];

                std::ranges::transform(buf, std::back_inserter(vMaterialIndices), [nStartIndex](auto elem) {
                    return elem + nStartIndex;
                });
            }

            ++nPrimIndex;
        }

        return true;
    }
    
} // namespace Ice
