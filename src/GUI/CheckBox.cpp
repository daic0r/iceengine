//
//  CheckBox.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 24.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/CheckBox.h>

#include <GUI/WidgetManager.h>
#include <GUI/ImageView.h>
#include <GUI/TextLabel.h>
#include <GUI/Constraints/MarginConstraint.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/TwoPointSizeConstraint.h>
#include <GUI/Constraints/DynamicConstraint.h>
#include <GUI/Fonts/TextStyle.h>
#include <System/SystemServices.h>
#include <iostream>
#include <Interfaces/IGraphicsSystem.h>

namespace Ice {

namespace GUI {

CheckBox::CheckBox(const std::string& strLabel, bool bChecked)
: m_strLabel{ strLabel },
m_bChecked{ bChecked }
{
    setIsTabStoppable(true);

    auto& wm = systemServices.getWidgetManager();
    
    //////////////////////////////////////////////
    // Load images
    std::string strImagePath = systemServices.getConfig()->prependAssetPath("GUI/CheckBox.png");
//    std::string strImagePath{ Config::BasePath };
//    strImagePath.append("/Assets/GUI/CheckBox.png");
    m_pImgChecked = std::make_shared<Image>(strImagePath);
    
    strImagePath = systemServices.getConfig()->prependAssetPath("GUI/CheckBoxUnchecked.png");
//    strImagePath = Config::BasePath;
//    strImagePath.append("/Assets/GUI/CheckBoxUnchecked.png");
    m_pImgUnchecked = std::make_shared<Image>(strImagePath);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    // Create checkbox image and assign lambda to toggle its state
    m_pImageView = static_cast<ImageView*>(wm.createWidget<ImageView, true>(this, m_pImgUnchecked, false));
    m_pImageView->setContentMode(ImageView::ContentMode::SCALE_TO_FIT);
    m_pImageView->setThemable(true);
    
    m_pImageView->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT));
    m_pImageView->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    m_pImageView->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(m_pImageView->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
    m_pImageView->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DynamicSizeConstraint>([](Widget* w) {
        return w->getSizeConstraint(OrthogonalAxis::VERTICAL)->getValue();;
    }));
    
    m_pImageView->setOnMouseButtonFunc([](Widget* w, MouseButton button, bool bPressed, bool& bHandled) {
        if (button == MouseButton::LEFT && !bPressed) {
            auto pImage = static_cast<ImageView*>(w);
            auto pCheckBox = static_cast<CheckBox*>(w->parent());
            pCheckBox->toggle();
            if (pCheckBox->checked()) {
                pImage->setImage(pCheckBox->m_pImgChecked);
            } else {
                pImage->setImage(pCheckBox->m_pImgUnchecked);
            }
        }
    });
    //////////////////////////////////////////////
    
    //////////////////////////////////////////////
    // Create label next to the image of the checkbox
    m_pTextLabel = static_cast<TextLabel*>(wm.createWidget<TextLabel, true>(this, strLabel/*, style*/));
    
    m_pTextLabel->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT, true, m_pImageView));
    m_pTextLabel->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    m_pTextLabel->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(m_pTextLabel->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT, true, this)));
    m_pTextLabel->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<TwoPointSizeConstraint>(m_pTextLabel->getPointConstraint(OrthogonalAxis::VERTICAL), std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM)));
    //////////////////////////////////////////////
}


}

}
