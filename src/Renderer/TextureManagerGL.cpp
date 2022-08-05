#include <Renderer/OpenGL/TextureManagerGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>

namespace Ice {

BaseTexture* TextureManagerGL::registerTexture(const std::string& strFile)
{
	TextureGL *pTex{ nullptr };
	bool bInserted;
	auto texIter = m_mTextures.find(strFile);
	if (texIter == m_mTextures.end()) {
		auto [iter, bSuccess] = m_mTextures.emplace(strFile, std::make_unique<TextureGL>());
		pTex = iter->second.get();
		pTex->setMinMagFilter(MinMagFilter::NEAREST);
		pTex->loadFromFile(strFile);
		bInserted = true;
	} else {
		pTex = texIter->second.get();
		bInserted = false;
	}
	return pTex;
}

BaseTexture* TextureManagerGL::getTexture(const std::string& strFile) noexcept
{
	auto iter = m_mTextures.find(strFile);
	if (iter != m_mTextures.cend()) {
		return iter->second.get();
	}
	return nullptr;
}

}