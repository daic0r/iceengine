#include <System/RenderMaterial.h>
#include <iostream>
#include <System/StreamTools.h>

namespace Ice {

	std::ostream& operator<<(std::ostream& os, const RenderMaterial& r) {
        StreamTools::writeStringToStream(os, r.m_strName);
        os << " ";
        StreamTools::writeStringToStream(os, r.m_strTextureFile);
        os << " ";
		os << r.m_ambient << " " << r.m_diffuse << " " << r.m_specular << " " << r.m_fSpecularExponent << " "
			<< r.m_fAlpha << " " << r.m_fRefractionIndex;
		return os;
	}

	std::istream& operator>>(std::istream& is, RenderMaterial& r) {
        StreamTools::readStringFromStream(is, r.m_strName);
        StreamTools::readStringFromStream(is, r.m_strTextureFile);
		is >> r.m_ambient >> r.m_diffuse >> r.m_specular >> r.m_fSpecularExponent >> r.m_fAlpha >> r.m_fRefractionIndex;
		return is;
	}
}
