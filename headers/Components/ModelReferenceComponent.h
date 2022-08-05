//
//  ModelReferenceComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelReferenceComponent_h
#define ModelReferenceComponent_h

#include <Components/ComponentIds.h>
#include <Entities/EntityManager.h>
#include <Entities/EntityComponentBuffer.h>
#include <Components/MeshComponent.h>
#include <System/StreamTools.h>
#include <iostream>
#include <Utils/serializables.h>

namespace Ice {

struct ModelReferenceComponent {
    static constexpr EntityComponentId id() noexcept { return MODEL_REFERENCE_COMPONENT; }
    
    Entity m_entity{ 0 };
    std::string m_strModelName;

    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(modelName, m_strModelName)
    END_DECLARE_SERIALIZABLE_ATTRIBS()

    ModelReferenceComponent() = default;
    ModelReferenceComponent(Entity e, std::string_view strName = "") : m_entity{ e }, m_strModelName{ strName } {
        if (strName.empty()) {
            entityManager.getComponent<MeshComponent>(e, [&strName, this](MeshComponent& comp) {
                m_strModelName = comp.name();
                });
        }
    }
};

inline std::ostream& operator<<(std::ostream& os, const ModelReferenceComponent& c) {
    os << ModelReferenceComponent::id() << " ";
    StreamTools::writeStringToStream(os, c.m_strModelName);
    return os;
}

inline std::istream& operator>>(std::istream& is, ModelReferenceComponent& c) {
    StreamTools::readStringFromStream(is, c.m_strModelName);
    auto e = EntityComponentBuffer<MeshComponent>::instance().findEntity([&c](const MeshComponent& mesh) {
        return mesh.name() == c.m_strModelName;
    }, entityManager.currentScene());
    if (e == 0)
        throw std::runtime_error("Didn't find mesh component named '" + c.m_strModelName + "'");
    c.m_entity = e;
    return is;
}

}

#endif /* ModelReferenceComponent_h */
