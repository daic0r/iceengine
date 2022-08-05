//
//  CheckBox.h
//  IceCraft
//
//  Created by Matthias Gruen on 24.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef CheckBox_h
#define CheckBox_h

#include <GUI/Widget.h>
#include <string>
#include <System/Image.h>

namespace Ice {

namespace GUI {

class TextLabel;
class ImageView;

class CheckBox : public Widget {
    bool m_bChecked{ false };
    std::string m_strLabel;
    std::shared_ptr<Image> m_pImgUnchecked;
    std::shared_ptr<Image> m_pImgChecked;
    
    TextLabel* m_pTextLabel{ nullptr };
    ImageView* m_pImageView{ nullptr };
    
public:
    CheckBox(const std::string&, bool);
    void toggle() noexcept { m_bChecked = !m_bChecked; }
    auto checked() const noexcept { return m_bChecked; }
    
    auto textLabel() const noexcept { return m_pTextLabel; }
    auto imageView() const noexcept { return m_pImageView; }
};

}

}


#endif /* CheckBox_h */
