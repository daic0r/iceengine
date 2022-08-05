//
//  Image.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 23.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <System/Image.h>
#include <System/BaseTexture.h>
#include <Renderer/OpenGL/TextureGL.h>


namespace Ice {

Image::Image(const std::string& strFile) noexcept
: m_strFile{ strFile }
{
#ifdef RENDERER_OPEN_GL
    m_pTexture = std::make_unique<TextureGL>();
#endif
    m_pTexture->loadFromFile(strFile);
}

}
