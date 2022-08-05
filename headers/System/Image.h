//
//  Image.h
//  IceCraft
//
//  Created by Matthias Gruen on 23.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Image_h
#define Image_h

#include <string>
#include <System/BaseTexture.h>
#include <memory>

namespace Ice {

class Image {
    std::string m_strFile;
    std::unique_ptr<BaseTexture> m_pTexture;

public:
    Image(const std::string& strFile) noexcept;
  
    auto width() const noexcept { return m_pTexture->width(); }
    auto height() const noexcept { return m_pTexture->height(); }
    std::string_view file() const noexcept { return m_strFile; }
    auto data() const noexcept { return m_pTexture.get(); }
};

}

#endif /* Image_h */
