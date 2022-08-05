#pragma once

#include <Interfaces/ITextureManager.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <memory>

namespace Ice {

class TextureManagerGL : public ITextureManager {
	std::unordered_map<std::string, std::unique_ptr<TextureGL>> m_mTextures;

public:
	// Inherited via ITextureManager
	BaseTexture* registerTexture(const std::string& strFile) override;
	BaseTexture* getTexture(const std::string& strFile) noexcept override;
};

}