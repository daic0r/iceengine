//
//  RenderMaterialsComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef RenderMaterialComponent_h
#define RenderMaterialComponent_h

#include <IceEngine.h>
#include <Components/ComponentIds.h>
#include <System/RenderMaterial.h>

#include <vector>
#include <map>
#include <string>

namespace Ice {

struct RenderMaterialsComponent {
private:
    std::map<std::string, RenderMaterial> m_mMaterials;

public:
    BEGIN_DECLARE_SERIALIZABLE_ATTRIBS()
    SERIALIZABLE_ATTRIB(materials, m_mMaterials)
    END_DECLARE_SERIALIZABLE_ATTRIBS()

    static constexpr EntityComponentId id() noexcept { return RENDER_MATERIALS_COMPONENT; }

    const RenderMaterial& getMaterial(const std::string&) const;
    inline auto& materials() noexcept { return m_mMaterials; }

	friend std::ostream& operator<<(std::ostream&, const RenderMaterialsComponent&);
	friend std::istream& operator>>(std::istream&, RenderMaterialsComponent&);
};

 std::ostream& operator<<(std::ostream&, const RenderMaterialsComponent&);
 std::istream& operator>>(std::istream&, RenderMaterialsComponent&);

}

#endif /* RenderMaterialsComponent_h */
