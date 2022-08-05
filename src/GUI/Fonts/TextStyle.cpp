#include <GUI/Fonts/TextStyle.h>

#include <System/SystemServices.h>
#include <Interfaces/ITextManager.h>
#include <Interfaces/IGraphicsSystem.h>
#include <GUI/Fonts/FontInfo.h>

namespace Ice {

namespace GUI {

const FontInfo* TextStyle::fontInfo() const noexcept {
    if (m_pFontInfo == nullptr)
        m_pFontInfo = &systemServices.getTextManager()->getFont(m_strFontName);
    return m_pFontInfo;
}

float TextStyle::getScaledPixelWidth() const noexcept
{
    const float fFactor = static_cast<float>(size()) / static_cast<float>(fontInfo()->size);
    const float fPixelWidth = fFactor * systemServices.getGraphicsSystem()->getHorizontalPixelScale();
    return fPixelWidth;
}

float TextStyle::getScaledPixelHeight() const noexcept
{
    const float fFactor = static_cast<float>(size()) / static_cast<float>(fontInfo()->size);
    const float fPixelHeight = fFactor * systemServices.getGraphicsSystem()->getVerticalPixelScale();
    return fPixelHeight;
}

/*!
* \brief
* [TextStyle::getScaledPixelDimensions]
*
* \author Matthias.Gruen
* \date 2021/01/07
* [1/7/2021 Matthias.Gruen]
*/
std::pair<float, float> TextStyle::getScaledPixelDimensions() const noexcept
{
    return { getScaledPixelWidth(), getScaledPixelHeight() };
}

float TextStyle::lineHeight() const noexcept {
	[[maybe_unused]] auto [fPixelWidth, fPixelHeight] = getScaledPixelDimensions();
	return static_cast<float>(fontInfo()->lineHeight) * fPixelHeight;
}

}

}
