//
//  TagComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TagComponent_h
#define TagComponent_h

#include <Components/ComponentIds.h>
#include <string>
#include <System/StreamTools.h>
#include <Utils/serializables.h>

namespace Ice {

struct TagComponent {
    static constexpr EntityComponentId id() noexcept { return TAG_COMPONENT; }
    
    std::string m_strTag;

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(tag, m_strTag)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

inline std::ostream& operator<<(std::ostream& os, const TagComponent& c) {
    os << TagComponent::id() << " ";
    StreamTools::writeStringToStream(os, c.m_strTag);
    return os;
}

inline std::istream& operator>>(std::istream& is, TagComponent& c) {
    StreamTools::readStringFromStream(is, c.m_strTag);
    return is;
}



}

#endif /* TagComponent_h */
