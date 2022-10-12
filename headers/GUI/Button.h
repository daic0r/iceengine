//
//  Button.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Button_h
#define Button_h

#include  <IceEngine.h>

#include <GUI/Widget.h>
#include <string>

namespace Ice {

class IWidgetRenderer;

namespace GUI {

class Button : public Widget {
  
    std::string m_strLabel;
    glm::vec4 m_oldColor;
    
public:
    
    Button(const std::string& = "");
    void render(IWidgetRenderer*);
};


}

}


#endif /* Button_h */
