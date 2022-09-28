#ifndef ModelImporterGlTF_h
#define ModelImporterGlTF_h

#include <string>
#include <Components/MeshComponent.h>
#include <Components/AnimatedMeshComponent.h>
#include <System/RenderMaterial.h>
#include <map>
#include <string_view>
#include <Components/SkeletonComponent.h>
#include <ModelAnimation/ModelAnimation.h>

namespace tinygltf {
    class Model;
}

namespace Ice
{
    class ModelImporterGlTF {
    
        void loadMeshAndMaterials(const tinygltf::Model&);
        void loadSkeleton(const tinygltf::Model&);
        void loadAnimations(const tinygltf::Model&);

    public:
        ModelImporterGlTF(std::string_view strFile);
        bool import();
        const auto& meshes() const noexcept { return m_mMeshes; }
        const auto& animatedMeshes() const noexcept { return m_mAniMeshes; }
        const auto& materials() const noexcept { return m_mMaterials; }
        const auto& animations() const noexcept { return m_mAnimations; }
        const auto& skeletonComponent() const noexcept { return m_skeleton; }

    private:
        std::string m_strFile{};
        std::map<std::string, MeshComponent> m_mMeshes;
        std::map<std::string, AnimatedMeshComponent> m_mAniMeshes;
        std::map<std::string, RenderMaterial> m_mMaterials;
        std::map<std::string, ModelAnimation> m_mAnimations;
        SkeletonComponent m_skeleton;
    };
} // namespace Ice



#endif