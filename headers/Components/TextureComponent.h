//
//  TextureComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TextureComponent_h
#define TextureComponent_h

#include <string>
#include <Components/ComponentIds.h>
#include <Entities/Entity.h>

namespace Ice {

class ITexture;

struct TextureComponent {
private:
    ITexture* m_pTexture{ nullptr };
    std::string m_strTextureName;
    
public:
    inline auto texture() const noexcept { return m_pTexture; }

    static constexpr EntityComponentId id() noexcept { return TEXTURE_COMPONENT; }
};

}

#endif /* TextureComponent_h */
