#ifndef ModelImporterGlTF_h
#define ModelImporterGlTF_h

#include <Components/MeshComponent.h>
#include <Components/AnimatedMeshComponent.h>
#include <System/RenderMaterial.h>
#include <Components/SkeletonComponent.h>
#include <ModelAnimation/ModelAnimation.h>
#include <ModelAnimation/Joint.h>

namespace tinygltf {
    class Model;
    class Mesh;
    class Skin;
    class Node;
}

namespace Ice
{
    class ModelImporterGlTF {
    
        void loadMeshAndMaterials(const tinygltf::Model&, const tinygltf::Mesh&);
        void loadSkeleton(const tinygltf::Model&, const tinygltf::Skin&, const tinygltf::Mesh&);
        void loadAnimations(const tinygltf::Model&, const tinygltf::Skin&);

        static glm::mat4 loadNodeTransform(const tinygltf::Node& jointNode) noexcept;
        static std::size_t nodeIndexToJointIndex(const tinygltf::Skin& skin, std::size_t nNodeIndex);
        static void addChildren(const tinygltf::Model& model, const tinygltf::Skin& skin, std::vector<Joint>& vJoints, Joint& j, const std::vector<int>& vChildren);

    public:
        ModelImporterGlTF(std::string_view strFile);
        bool import();
        const auto& meshes() const noexcept { return m_mMeshes; }
        const auto& animatedMeshes() const noexcept { return m_mAniMeshes; }
        const auto& materials() const noexcept { return m_mMaterials; }
        const auto& animations() const noexcept { return m_mAnimations; }
        const auto& skeletonComponents() const noexcept { return m_mSkeletons; }

    private:
        std::string m_strFile{};
        std::map<std::string, MeshComponent> m_mMeshes;
        std::map<std::string, AnimatedMeshComponent> m_mAniMeshes;
        std::map<std::string, SkeletonComponent> m_mSkeletons;
        std::map<std::string, ModelAnimation> m_mAnimations;
        std::map<std::string, RenderMaterial> m_mMaterials;
    };
} // namespace Ice



#endif