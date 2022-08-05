//
//  ModelPreview.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 27.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/ModelPreview.h>

#include <Interfaces/IWidgetRenderer.h>

namespace Ice {

namespace GUI {

void ModelPreview::render(IWidgetRenderer* pRenderer) {
    pRenderer->renderWidget(this);
    setGraphicsDirty(false);
}


}

}
