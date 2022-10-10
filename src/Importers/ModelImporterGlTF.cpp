#include <Importers/ModelImporterGlTF.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
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

    glm::mat4 ModelImporterGlTF::loadNodeTransform(const tinygltf::Node& jointNode) noexcept {
        glm::mat4 matTrans{1.0f}, matRot{1.0f}, matScale{1.0f};
        if (jointNode.rotation.size() > 0) {
            glm::quat qRot = glm::make_quat(&jointNode.rotation.at(0));
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

    std::size_t ModelImporterGlTF::nodeIndexToJointIndex(const tinygltf::Skin& skin, std::size_t nNodeIndex) {
        return std::distance(skin.joints.cbegin(), std::ranges::find_if(skin.joints, [nNodeIndex](int node) { return node == nNodeIndex; }));
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
        const auto path = std::filesystem::path{ m_strFile };
        const auto strExt = std::string{ path.extension() };
        if (strExt == ".gltf") {
            if (!gltf_ctx.LoadASCIIFromFile(&model, &strErr, &strWarn, m_strFile)) {
                throw std::runtime_error("glTF couldn't be loaded");
            }
        } else if (strExt == ".glb") {
            if (!gltf_ctx.LoadBinaryFromFile(&model, &strErr, &strWarn, m_strFile)) {
                throw std::runtime_error("glTF couldn't be loaded");
            }
        } else {
            throw std::runtime_error("Invalid file extension (must be either .gltf or .glb");
        }

        const auto instanceNodeIter = std::ranges::find_if(model.nodes, [](const tinygltf::Node& node) { return node.mesh > -1; });
        const auto& mesh = model.meshes.at(instanceNodeIter->mesh);
        loadMeshAndMaterials(model, mesh);
        
        if (instanceNodeIter->skin > -1) {
            const auto& skin = model.skins.at(instanceNodeIter->skin);
            loadSkeleton(model, skin, mesh);
            loadAnimations(model, skin);
        }

        return true;
    }

    void ModelImporterGlTF::loadMeshAndMaterials(const tinygltf::Model& model, const tinygltf::Mesh& mesh) {
        // Materials
        std::vector<RenderMaterial> vMaterials;
        vMaterials.reserve(model.materials.size());

        for (const auto& material : model.materials) {
            RenderMaterial mat;
            mat.setDiffuse(glm::vec3{ material.pbrMetallicRoughness.baseColorFactor[0], material.pbrMetallicRoughness.baseColorFactor[1], material.pbrMetallicRoughness.baseColorFactor[2] });
            mat.setSpecular(mat.diffuse());
            mat.setAmbient(mat.diffuse());
            mat.setName(material.name);
            mat.setSpecularExponent(32.0f);
            vMaterials.push_back(mat);
            m_mMaterials.emplace(material.name, mat);
        }
        ///////////////////////////////////////////////////////////////////////

            auto& mCurObject = m_mMeshes[mesh.name];

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
                        
                        // We're doing this here in case this is not an animated mesh
                        // In that case we'll not just create an empty entry in the map
                        auto& mCurAniObject = m_mAniMeshes[mesh.name];
                        const auto buf = makeTypedBufferFromBufferView<glm::i8vec4>(model, accessor);
                        std::ranges::transform(buf, std::back_inserter(mCurAniObject.m_vJointIds), [](const glm::i8vec4& elem) {
                            return glm::ivec4{ elem[0], elem[1], elem[2], elem[3] };
                        });
                    }
                    else
                    if (strAttrName == "WEIGHTS_0") {
                        assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);
                        assert(accessor.type == TINYGLTF_TYPE_VEC4);

                        // We're doing this here in case this is not an animated mesh
                        // In that case we'll not just create an empty entry in the map
                        auto& mCurAniObject = m_mAniMeshes[mesh.name];
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
    }

    void ModelImporterGlTF::addChildren(const tinygltf::Model& model, const tinygltf::Skin& skin, std::vector<Joint>& vJoints, Joint& j, const std::vector<int>& vChildren) {
        for (auto nChild : vChildren) {
            const auto& jointNode = model.nodes.at(nChild);
            auto& child = vJoints.at(nodeIndexToJointIndex(skin, nChild));
            addChildren(model, skin, vJoints, child, jointNode.children);
        }
        
        for (auto nChild : vChildren) {
            auto& child = vJoints.at(nodeIndexToJointIndex(skin, nChild));
            j.addChild(child);
        }
    }
 
    void ModelImporterGlTF::loadSkeleton(const tinygltf::Model& model, const tinygltf::Skin& skin, const tinygltf::Mesh& mesh) {
        auto vInvBindMatrices = makeTypedBufferFromBufferView<glm::mat4>(model, model.accessors.at(skin.inverseBindMatrices));

        std::vector<Joint> vJoints;
        vJoints.resize(skin.joints.size());
        
        std::size_t nIdx{};
        for (auto nJoint : skin.joints) {
            const auto& jointNode = model.nodes.at(nJoint);

            const auto bindTransform = loadNodeTransform(jointNode);

            auto& j = vJoints.at(nIdx);
            j.setId(nIdx);
            j.setName(jointNode.name);
            j.setBindTransform(bindTransform);
            j.setInvBindTransform(vInvBindMatrices.at(nIdx));
            ++nIdx;
        }

        auto& skeleton = m_mSkeletons[mesh.name];
        skeleton.m_rootJoint = vJoints.at(0);
        addChildren(model, skin, vJoints, skeleton.m_rootJoint, model.nodes.at(skin.joints.at(0)).children);

        if (const auto iter = std::ranges::find_if(model.nodes, [&skin](const tinygltf::Node& node) { return node.name == skin.name; }); iter != model.nodes.end()) {
            skeleton.m_invBindShapeMatrix = glm::inverse(loadNodeTransform(*iter));
        } else {
            skeleton.m_invBindShapeMatrix = glm::mat4{1.0f};
        }

        for (auto& [strName, animatedMesh] : m_mAniMeshes) {
            animatedMesh.m_nNumJoints = skin.joints.size();
        }
    }

    void ModelImporterGlTF::loadAnimations(const tinygltf::Model& model, const tinygltf::Skin& skin) {
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
                    });
                }
                else
                if (channel.target_path == "scale") {
                    assert(jointTransform.vScalings.size() == 0);
                    jointTransform.vScalings = makeTypedBufferFromBufferView<glm::vec3>(model, accessorOutput);
                }
            }

            float fLengthSeconds{};
            for (const auto& [nJointNodeId, jointTransforms] : mJointTransforms) {
                const auto nJointIndex = nodeIndexToJointIndex(skin, nJointNodeId);
                auto& jointAnimation = mCurAnimation.jointAnimations()[nJointIndex];
                assert(jointTransforms.vTimepoints.size() == jointTransforms.vTranslations.size()); 
                assert(jointTransforms.vTimepoints.size() == jointTransforms.vRotations.size()); 
                assert(jointTransforms.vTimepoints.size() == jointTransforms.vScalings.size()); 

                const auto fLengthThisJoint = *std::prev(jointTransforms.vTimepoints.end());
                if (fLengthThisJoint > fLengthSeconds)
                    fLengthSeconds = fLengthThisJoint;

                for (std::size_t i{}; i < jointTransforms.vTimepoints.size(); ++i) {
                    JointTransform tf{ jointTransforms.vTranslations.at(i), jointTransforms.vRotations.at(i), jointTransforms.vScalings.at(i) };
                    jointAnimation.jointTransforms().emplace(jointTransforms.vTimepoints.at(i), tf);
                }
            }

            mCurAnimation.setLengthSeconds(fLengthSeconds);
        }
    }
    
} // namespace Ice
