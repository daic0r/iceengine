//
//  SharedComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.04.21.
//  Copyright © 2021 Matthias Gruen. All rights reserved.
//

#ifndef SharedComponent_h
#define SharedComponent_h

#include <Components/ComponentIds.h>
#include <System/StreamTools.h>
#include <iostream>
#include <Utils/serializables.h>

namespace Ice {

template<typename ComponentType>
struct SharedComponent {
    static constexpr EntityComponentId id() noexcept { return SHARED_COMPONENT_BASE + ComponentType::id(); }
    
    ComponentType* m_pComponent{ nullptr };
    Entity m_refEnt{ INVALID_ENTITY_ID };
    std::string m_strReferenceEntityTag;

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(referenceEntityTag, m_strReferenceEntityTag)
    END_DECLARE_SERIALIZABLE_ATTRIBS()
};

template<typename ComponentType> std::ostream& operator<<(std::ostream& os, const SharedComponent<ComponentType>& c) {
    os << SharedComponent<ComponentType>::id() << " ";
    StreamTools::writeStringToStream(os, c.m_strReferenceEntityTag);
    return os;
}

template<typename ComponentType> std::istream& operator>>(std::istream& is, SharedComponent<ComponentType>& c) {
    StreamTools::readStringFromStream(is, c.m_strReferenceEntityTag);
    return is;
}

}

#endif /* SharedComponent_h */
