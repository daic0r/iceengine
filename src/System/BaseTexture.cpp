#include <System/BaseTexture.h>
#include <SDL2/SDL.h>
#include <stdexcept>


namespace Ice {

void BaseTexture::registerSubTexture(const std::string& name, const SDL_Rect& rect, bool bHasTransparency /*= false*/)
{
	sSubTextureAttributes attr;
	attr.bHasTransparency = bHasTransparency;

	auto uvFrom = getTexCoord(rect.x, rect.y + (rect.h - 1));
	auto uvTo = getTexCoord(rect.x + (rect.w - 1), rect.y);
	attr.vTexCoords = std::vector<float>{
		uvFrom.first, uvFrom.second,
		uvTo.first, uvFrom.second,
		uvTo.first, uvTo.second,
		uvFrom.first, uvTo.second,
	};

	m_mSubTextures.emplace(name, attr);
}

/*!
* \brief
* [BaseTexture::cellWidth]
*
* \author matthias.gruen
* \date 2020/10/20
* [10/20/2020 matthias.gruen]
*/
float BaseTexture::cellWidth() const noexcept
{
	if (m_fCellWidth < 0.0f) {
		m_fCellWidth = static_cast<float>(width()) / static_cast<float>(cols()) * m_fTexelWidth;
	}
	return m_fCellWidth;
}

/*!
* \brief
* [BaseTexture::cellHeight]
*
* \author matthias.gruen
* \date 2020/10/20
* [10/20/2020 matthias.gruen]
*/
float BaseTexture::cellHeight() const noexcept
{
	if (m_fCellHeight < 0.0f) {
		m_fCellHeight = static_cast<float>(height()) / static_cast<float>(rows()) * m_fTexelHeight;
	}
	return m_fCellHeight;
}

const std::vector<float>& BaseTexture::getTexCoord(const std::string& name) const
{
	auto subTexIter = m_mSubTextures.find(name);
	if (subTexIter == m_mSubTextures.end())
		throw std::runtime_error("Sub texture not found");

	return subTexIter->second.vTexCoords;
}

/*!
* \brief
* [BaseTexture::getTexCoord]
*
* \author matthias.gruen
* \date 2020/10/20
* [10/20/2020 matthias.gruen]
*/
std::pair<float, float> BaseTexture::getTexCoord(int x, int y) const noexcept
{
	float outx = (float)x * texelWidth() + (texelWidth() / 2.0f);
	float outy = (float)(height() - y) * texelHeight() + (texelHeight() / 2.0f);

	return std::make_pair(outx, outy);
}

/*!
* \brief
* [BaseTexture::hasTransparency]
*
* \author matthias.gruen
* \date 2020/10/20
* [10/20/2020 matthias.gruen]
*/
bool BaseTexture::hasTransparency(const std::string& name) const
{
	auto subTexIter = m_mSubTextures.find(name);
	if (subTexIter == m_mSubTextures.end())
		throw std::runtime_error("Sub texture not found");

	return subTexIter->second.bHasTransparency;
}

}
