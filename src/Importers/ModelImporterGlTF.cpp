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
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <ModelAnimation/JointAnimation.h>
#include <ModelAnimation/JointTransform.h>
#include <glm/gtc/type_ptr.hpp>

namespace Ice
{
    template<typename T> static auto makeTypedBufferFromBufferView(const tinygltf::Model& model, const tinygltf::Accessor& accessor) {
        const auto& view = model.bufferViews.at(accessor.bufferView);

        std::vector<T> vRet;
        vRet.resize(accessor.count);
        const auto nByteSize = accessor.count * sizeof(T);
        std::memcpy(&vRet[0], &model.buffers.at(view.buffer).data[view.byteOffset + accessor.byteOffset], nByteSize);
        return vRet;
    };

    static glm::mat4 loadNodeTransform(const tinygltf::Node& jointNode) {
        glm::mat4 matTrans{1.0f}, matRot{1.0f}, matScale{1.0f};
        if (jointNode.rotation.size() > 0) {
            glm::quat qRot = glm::make_quat(&jointNode.rotation.at(0));
            //glm::quat qRot{ jointNode.rotation.at(3), jointNode.rotation.at(0), jointNode.rotation.at(1), jointNode.rotation.at(2) };
            matRot = glm::mat4{ qRot };
        }
        if (jointNode.scale.size() > 0) {
            matScale = glm::scale(glm::mat4{1.0f}, glm::vec3{ jointNode.scale.at(0), jointNode.scale.at(1), jointNode.scale.at(2) });
        }
        if (jointNode.translation.size() > 0) {
            matTrans = glm::translate(glm::mat4{1.0f}, glm::vec3{ jointNode.translation.at(0), jointNode.translation.at(1), jointNode.translation.at(2) });
        }
        return matTrans * matRot * matScale;
    }

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

        loadMeshAndMaterials(model);
        loadSkeleton(model);
        loadAnimations(model);

        return true;
    }

    void ModelImporterGlTF::loadMeshAndMaterials(const tinygltf::Model& model) {
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

                    if (strAttrName == "POSITION") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                        assert(accessor.type == TINYGLTF_TYPE_VEC3);

                        const auto buf = makeTypedBufferFromBufferView<glm::vec3>(model, accessor);
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

                        const auto buf = makeTypedBufferFromBufferView<glm::vec3>(model, accessor);
                        mCurObject.normals().insert(mCurObject.normals().end(), buf.begin(), buf.end());
                    }
                    else
                    if (strAttrName == "JOINTS_0") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE);
                        assert(accessor.type == TINYGLTF_TYPE_VEC4);
                        
                        const auto buf = makeTypedBufferFromBufferView<glm::i8vec4>(model, accessor);
                        std::ranges::transform(buf, std::back_inserter(mCurAniObject.m_vJointIds), [](const glm::i8vec4& elem) {
                            return glm::ivec4{ elem[0], elem[1], elem[2], elem[3] };
                        });
                    }
                    else
                    if (strAttrName == "WEIGHTS_0") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                        assert(accessor.type == TINYGLTF_TYPE_VEC4);

                        const auto buf = makeTypedBufferFromBufferView<glm::vec4>(model, accessor);
                        for (const auto& v : buf) {
                            assert(Math::equal(v[0] + v[1] + v[2] + v[3], 1.0f));
                        }
                        mCurAniObject.m_vWeights.insert(mCurAniObject.m_vWeights.end(), buf.begin(), buf.end());
                    }
                } // attributes

                mCurObject.extents().minPoint = minVertex;
                mCurObject.extents().maxPoint = maxVertex;

                const auto& indexAccessor = model.accessors.at(prim.indices);
                assert(indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT);
                const auto buf = makeTypedBufferFromBufferView<unsigned short>(model, indexAccessor);
                auto& vMaterialIndices = mCurObject.materialIndices()[vMaterials.at(prim.material).name()];

                std::ranges::transform(buf, std::back_inserter(vMaterialIndices), [nStartIndex](auto elem) {
                    return elem + nStartIndex;
                });
            }

            ++nPrimIndex;
        }
    }

    void addChildren(const tinygltf::Model& model, const tinygltf::Skin& skin, std::vector<Joint>& vJoints, Joint& j, const std::vector<int>& vChildren) {
        for (auto nChild : vChildren) {
            const auto& jointNode = model.nodes.at(nChild);
            addChildren(model, skin, vJoints, vJoints.at(nChild), jointNode.children);
        }
        
        for (auto nChild : vChildren) {
            const auto& jointNode = model.nodes.at(skin.joints.at(nChild));
            j.addChild(vJoints.at(nChild));
        }
    }
 
    void ModelImporterGlTF::loadSkeleton(const tinygltf::Model& model) {
        if (model.skins.size() > 1)
            throw std::runtime_error("Only 1 skeleton supported!");
        if (model.skins.size() == 0)
            return;

        const auto& skin = model.skins.front();
        auto vInvBindMatrices = makeTypedBufferFromBufferView<glm::mat4>(model, model.accessors.at(skin.inverseBindMatrices));

        std::vector<Joint> vJoints;
        vJoints.resize(skin.joints.size());
        
        for (auto nJoint : skin.joints) {
            const auto& jointNode = model.nodes.at(nJoint);

            const auto bindTransform = loadNodeTransform(jointNode);

            auto& j = vJoints.at(nJoint);// { nJoint, jointNode.name, glm::inverse(bindTransform) };
            j.setId(nJoint);
            j.setName(jointNode.name);
            j.setBindTransform(bindTransform);
            j.setInvBindTransform(vInvBindMatrices.at(nJoint));
            //j.setInvBindTransform(glm::inverse(bindTransform));
        }

        m_skeleton.m_rootJoint = vJoints.at(skin.joints.at(0));
        addChildren(model, skin, vJoints, m_skeleton.m_rootJoint, model.nodes.at(skin.joints.at(0)).children);

        if (const auto iter = std::ranges::find_if(model.nodes, [&skin](const tinygltf::Node& node) { return node.name == skin.name; }); iter != model.nodes.end()) {
            m_skeleton.m_invBindShapeMatrix = glm::inverse(loadNodeTransform(*iter));
        } else {
            m_skeleton.m_invBindShapeMatrix = glm::mat4{1.0f};
        }

        for (auto& [strName, animatedMesh] : m_mAniMeshes) {
            animatedMesh.m_nNumJoints = skin.joints.size();
        }
    }

    void ModelImporterGlTF::loadAnimations(const tinygltf::Model& model) {
        struct sJointKeyframeTransform {
            std::vector<float> vTimepoints;
            std::vector<glm::vec3> vTranslations;
            std::vector<glm::quat> vRotations;
            std::vector<glm::vec3> vScalings;
        };
        for (const auto& ani : model.animations) {
            auto& mCurAnimation = m_mAnimations[ani.name];

            std::map<int, sJointKeyframeTransform> mJointTransforms;

            for (const auto& channel : ani.channels) {
                const auto& sampler = ani.samplers.at(channel.sampler);
                const auto& accessorOutput = model.accessors.at(sampler.output);
                auto& jointTransform = mJointTransforms[channel.target_node];

                if (jointTransform.vTimepoints.empty())
                    jointTransform.vTimepoints = makeTypedBufferFromBufferView<float>(model, model.accessors.at(sampler.input));

                if (channel.target_path == "translation") {
                    assert(jointTransform.vTranslations.size() == 0);
                    jointTransform.vTranslations = makeTypedBufferFromBufferView<glm::vec3>(model, accessorOutput);
                }
                else
                if (channel.target_path == "rotation") {
                    const auto vRot = makeTypedBufferFromBufferView<std::array<float, 4>>(model, accessorOutput);
                    assert(jointTransform.vRotations.size() == 0);
                    jointTransform.vRotations.reserve(vRot.size());
                    std::ranges::transform(vRot, std::back_inserter(jointTransform.vRotations), [](const std::array<float, 4>& elem) {
                        return glm::make_quat(&elem[0]);
                        //return glm::quat{ elem[3], elem[0], elem[1], elem[2] }; 
                    });
                }
                else
                if (channel.target_path == "scale") {
                    assert(jointTransform.vScalings.size() == 0);
                    jointTransform.vScalings = makeTypedBufferFromBufferView<glm::vec3>(model, accessorOutput);
                }
            }

            float fLengthSeconds{};
            for (const auto& [nJointId, jointTransforms] : mJointTransforms) {
                auto& jointAnimation = mCurAnimation.jointAnimations()[nJointId];
                assert(jointTransforms.vTimepoints.size() == jointTransforms.vTranslations.size()); 
                assert(jointTransforms.vTimepoints.size() == jointTransforms.vRotations.size()); 
                assert(jointTransforms.vTimepoints.size() == jointTransforms.vScalings.size()); 

                const auto fLengthThisJoint = *std::prev(jointTransforms.vTimepoints.end());
                if (fLengthThisJoint > fLengthSeconds)
                    fLengthSeconds = fLengthThisJoint;

                for (std::size_t i{}; i < jointTransforms.vTimepoints.size(); ++i) {
                    /*
                    const auto matScale = glm::scale(glm::mat4{1.0f}, jointTransforms.vScalings.at(i));
                    const auto matRot = glm::mat4{ jointTransforms.vRotations.at(i) };
                    const auto matTransl = glm::translate(glm::mat4{1.0f}, jointTransforms.vTranslations.at(i));
                    */
                    JointTransform tf{ jointTransforms.vTranslations.at(i), jointTransforms.vRotations.at(i), jointTransforms.vScalings.at(i) };
                    jointAnimation.jointTransforms().emplace(jointTransforms.vTimepoints.at(i), tf); // matTransl * matRot * matScale);
                }
            }

            mCurAnimation.setLengthSeconds(fLengthSeconds);
        }
    }
    
} // namespace Ice
