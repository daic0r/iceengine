#ifndef ModelImporterGlTF_h
#define ModelImporterGlTF_h

#include <string>
#include <Components/MeshComponent.h>
#include <Components/AnimatedMeshComponent.h>
#include <System/RenderMaterial.h>
#include <map>
#include <string_view>

namespace Ice
{
    class ModelImporterGlTF {
    public:
        ModelImporterGlTF(std::string_view strFile);
        bool import();
        const auto& meshes() const noexcept { return m_mMeshes; }
        const auto& animatedMeshes() const noexcept { return m_mAniMeshes; }
        const auto& materials() const noexcept { return m_mMaterials; }

    private:
        std::string m_strFile{};
        std::map<std::string, MeshComponent> m_mMeshes;
        std::map<std::string, AnimatedMeshComponent> m_mAniMeshes;
        std::map<std::string, RenderMaterial> m_mMaterials;
    };
} // namespace Ice



#endif