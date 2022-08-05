//
//  DropDown.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 30.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/DropDown.h>
#include <System/SystemServices.h>
#include <GUI/StackPanel.h>
#include <GUI/WidgetManager.h>
#include <GUI/Button.h>
#include <GUI/ImageView.h>
#include <GUI/WidgetView.h>
#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Constraints/DirectSizeConstraint.h>
#include <GUI/Constraints/TwoPointSizeConstraint.h>
#include <GUI/TextLabel.h>
#include <System/Percent.h>
#include <iostream>

namespace Ice {

namespace GUI {

DropDown::DropDown() {
    setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    setIsTabStoppable(true);

    auto& wm = systemServices.getWidgetManager();

    /////////////////////
    // Arrow button
    m_pDropDownButton = static_cast<Button*>(wm.createWidget<Button, true>(this));
    m_pDropDownButton->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    m_pDropDownButton->setColor(glm::vec4{ .5f, .5f, .5f, .95f });

    m_pDropDownButton->setPointConstraint(HorizontalAlignment::RIGHT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::RIGHT));
    m_pDropDownButton->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP));
    m_pDropDownButton->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    m_pDropDownButton->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Pixels>>(buttonSize()));
    
    std::string strImagePath = systemServices.getConfig()->prependAssetPath("GUI/ScrollButtonDown.png");
    //strImagePath.append("/Assets/GUI/ScrollButtonDown.png");

    auto pImage = static_cast<ImageView*>(wm.createWidget<ImageView, true>(m_pDropDownButton, strImagePath));
    pImage->setContentMode(ImageView::ContentMode::FILL);
    pImage->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pImage->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct));
    pImage->setThemable(true);
    //////////////////
    
    ////////////////////
    // Stack panel
    m_pStackPanel = static_cast<VerticalStackPanel*>(wm.createWidget<VerticalStackPanel, true>(this));
    m_pStackPanel->setDefaultHorizontalAlignment(HorizontalAlignment::LEFT);
    m_pStackPanel->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    m_pStackPanel->setColor(color());
    m_pStackPanel->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT, false));
    m_pStackPanel->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::BOTTOM, false));
    m_pStackPanel->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(m_pStackPanel->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT, false, m_pDropDownButton)));
    m_pStackPanel->setVisible(false);
    m_pStackPanel->setClipFrameFunc([](const Widget* w) {
        return w->extents();
    });
    
    m_pView = static_cast<WidgetView*>(wm.createWidget<WidgetView, true>(this));
    m_pView->setPadding(0.0f, 0.0f, 0.0f, 0.0f);
    m_pView->setColor(glm::vec4{ 0.0f, 0.0f, 0.0f, 0.0f });
    m_pView->setPointConstraint(HorizontalAlignment::LEFT, std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT, false));
    m_pView->setPointConstraint(VerticalAlignment::TOP, std::make_unique<VerticalAlignmentConstraint>(VerticalAlignment::TOP, false));
    m_pView->setSizeConstraint(OrthogonalAxis::HORIZONTAL, std::make_unique<TwoPointSizeConstraint>(m_pView->getPointConstraint(OrthogonalAxis::HORIZONTAL), std::make_unique<HorizontalAlignmentConstraint>(HorizontalAlignment::LEFT, false, m_pDropDownButton)));
    m_pView->setSizeConstraint(OrthogonalAxis::VERTICAL, std::make_unique<DirectSizeConstraint<Percent>>(100.0_pct, false));
    
    ////////////////////
    setOnMouseButtonFunc([](Widget* w, MouseButton button, bool bPressed, bool& bHandled) {
        // Item was selected for example, abort
        if (bHandled)
            return;
        
        if (button == MouseButton::LEFT && !bPressed) {
            auto pDropDown = static_cast<DropDown*>(w);
            pDropDown->m_pStackPanel->setVisible(!pDropDown->m_pStackPanel->visible());
        }
    });

	setOnFocusLostFunc([](Widget* w) {
        DropDown* pDropDown = static_cast<DropDown*>(w);
        pDropDown->stackPanel()->setVisible(false);
        std::cout << "Drop down lost focus" << std::endl;
    });
    m_bInitialized = true;
}

void DropDown::addChild(Widget* w, size_t nDrawOrder) {
    if (!m_bInitialized)
        Widget::addChild(w);
    else {
        m_pStackPanel->addChild(w, nDrawOrder);
        w->setIsTabStoppable(false);
        
        w->setOnMouseEnterFunc([](Widget *w, bool& bHandled) {
            DropDown* pDropDown = static_cast<DropDown*>(w->parent()->parent());
            pDropDown->m_mOldColors[w] = w->color();
            w->setColor(glm::vec4{ 0.0f, 0.0f, 1.0f, 0.8f });
        });

        w->setOnMouseLeaveFunc([](Widget* w, bool& bHandled) {
            DropDown* pDropDown = static_cast<DropDown*>(w->parent()->parent());
            w->setColor(pDropDown->m_mOldColors[w]);
        });
        
        w->setOnMouseButtonFunc([](Widget* w, MouseButton button, bool bPressed, bool& bHandled) {
            DropDown* pDropDown = static_cast<DropDown*>(w->parent()->parent());
            Widget* pStackPanel = w->parent();
            if (button == MouseButton::LEFT && !bPressed) {
                pDropDown->m_pSelected = w;
                pDropDown->m_pView->setWidget(w);
                pStackPanel->setVisible(false);
            }
            bHandled = true;
        });
    }
}

}

}
