#pragma once

#include <IceEngine.h>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <System/RenderMaterial.h>

namespace Ice {

struct MeshComponent;

class ModelImporterOBJ
{
	std::string m_strFile;
	std::map<std::string, RenderMaterial> m_mMaterials;
    std::map<std::string, std::vector<glm::vec3>> m_mPaths;

public:
	ModelImporterOBJ(const std::string& strFile);
    
	const auto& materials() const noexcept { return m_mMaterials; }
    const auto& paths() const noexcept { return m_mPaths; }
	bool import(std::map<std::string, MeshComponent>&) noexcept;
    
private:

	bool loadMTL(const std::string& strFile);
};

}
