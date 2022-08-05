#include <System/Types.h>
#include <System/Tools.h>

#include <System/StreamTools.h>

namespace Ice {

void StreamTools::writeStringToStream(std::ostream& os, const std::string& str) {
    os << "'" << str << "'";
}

bool StreamTools::readStringFromStream(std::istream& is, std::string& str) {
    bool bRet = (bool)(is >> str);
    if (!bRet)
        return false;
    if (str.length() > 1)
        str = str.substr(1, str.length()-2);
    return bRet;
}

std::ostream& operator<<(std::ostream& os, Ice::OrthogonalDirection d) {
    os << Tools::direction2string(d);
    return os;
}

std::istream& operator>>(std::istream& is, Ice::OrthogonalDirection& d) {
    std::string strData;
    if (is >> strData)
		d = Tools::string2direction(strData);
    return is;
}

std::ostream& operator<<(std::ostream& os, EdgeOrientation o) {
    os << Tools::edgeOrientation2string(o);
    return os;
}

std::istream& operator>>(std::istream& is, EdgeOrientation& o) {
    std::string strData;
    if (is >> strData)
		o = Tools::string2edgeOrientation(strData);
    return is;
}

//std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
//    return ::operator<<(os, v);
//}
//
//std::istream& operator>>(std::istream& is, glm::vec2& v) {
//    return ::operator>>(is, v);
//}
//
//std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
//    return ::operator<<(os, v);
//}
//
//std::istream& operator>>(std::istream& is, glm::vec3& v) {
//    return ::operator>>(is, v);
//}

}

// Needed by RenderMaterial::operator<<
namespace glm {
    std::ostream& operator<<(std::ostream& os, const glm::vec2& v) {
        os << v[0] << " " << v[1];
        return os;
    }

    std::istream& operator>>(std::istream& is, glm::vec2& v) {
        is >> v[0] >> v[1];
        return is;
    }

    std::ostream& operator<<(std::ostream& os, const glm::vec3& v) {
        os << v[0] << " " << v[1] << " " << v[2];
        return os;
    }

    std::istream& operator>>(std::istream& is, glm::vec3& v) {
        is >> v[0] >> v[1] >> v[2];
        return is;
    }
}
