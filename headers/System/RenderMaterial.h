#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <Utils/serializables.h>

namespace Ice {

class RenderMaterial
{
	std::string m_strName;
    std::string m_strTextureFile;
	glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
	glm::vec3 m_specular;
	float m_fSpecularExponent{ .0f };
	float m_fAlpha{ 1.0f };
	float m_fRefractionIndex{ 1.0f };

	BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
	SERIALIZABLE_ATTRIB(name, m_strName)
	SERIALIZABLE_ATTRIB(textureFile, m_strTextureFile)
	SERIALIZABLE_ATTRIB(ambient, m_ambient)
	SERIALIZABLE_ATTRIB(diffuse, m_diffuse)
	SERIALIZABLE_ATTRIB(specular, m_specular)
	SERIALIZABLE_ATTRIB(specularExponent, m_fSpecularExponent)
	SERIALIZABLE_ATTRIB(alpha, m_fAlpha)
	SERIALIZABLE_ATTRIB(refactionIndex, m_fRefractionIndex)
	END_DECLARE_SERIALIZABLE_ATTRIBS()

	friend inline void from_json(const nlohmann::json& j, RenderMaterial& m) {
		m.m_strName = j.at("name").get<std::string>();
		m.m_strTextureFile = j.at("textureFile").get<std::string>();
		m.m_ambient = j.at("ambient").get<glm::vec3>();
		m.m_diffuse = j.at("diffuse").get<glm::vec3>();
		m.m_specular = j.at("specular").get<glm::vec3>();
		m.m_fSpecularExponent = j.at("specularExponent").get<float>();
		m.m_fAlpha = j.at("alpha").get<float>();
		m.m_fRefractionIndex = j.at("refractionIndex").get<float>();
	}

	friend inline void to_json(nlohmann::json& j, const RenderMaterial& m) {
		j.emplace("name", m.m_strName);
		j.emplace("textureFile", m.m_strTextureFile);
		j.emplace("ambient", m.m_ambient);
		j.emplace("diffuse", m.m_diffuse);
		j.emplace("specular", m.m_specular);
		j.emplace("specularExponent", m.m_fSpecularExponent);
		j.emplace("alpha", m.m_fAlpha);
		j.emplace("refractionIndex", m.m_fRefractionIndex);
	}

public:
    //RenderMaterial(const RenderMaterial&) = default;
    //RenderMaterial& operator=(const RenderMaterial&) = default;
    
	const std::string& name() const noexcept { return m_strName; }
    const std::string& textureFile() const noexcept { return m_strTextureFile; }
	const glm::vec3& ambient() const noexcept { return m_ambient; }
	const glm::vec3& diffuse() const noexcept { return m_diffuse; }
	const glm::vec3& specular() const noexcept { return m_specular; }
	const float& specularExponent() const noexcept { return m_fSpecularExponent; }
	const float& alpha() const noexcept { return m_fAlpha; }
	const float& refractionIndex() const noexcept { return m_fRefractionIndex; }
    
	void setName(const std::string& strName) noexcept { m_strName = strName; }
    void setTextureFile(const std::string& strTexId) noexcept { m_strTextureFile = strTexId; }
	void setAmbient(const glm::vec3& ambient) noexcept { m_ambient = ambient; }
	void setDiffuse(const glm::vec3& diffuse) noexcept { m_diffuse = diffuse; }
	void setSpecular(const glm::vec3& specular) noexcept { m_specular = specular; }
	void setSpecularExponent(float f) noexcept { m_fSpecularExponent = f; }
	void setAlpha(float f) noexcept { m_fAlpha = f; }
	void setRefractionIndex(float f) noexcept { m_fRefractionIndex = f; }

	friend std::ostream& operator<<(std::ostream&, const RenderMaterial&);
	friend std::istream& operator>>(std::istream&, RenderMaterial&);
};

std::ostream& operator<<(std::ostream&, const RenderMaterial&);
std::istream& operator>>(std::istream&, RenderMaterial&);

}