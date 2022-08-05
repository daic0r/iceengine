//
//  AlignmentConstraint.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 16.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GUI/Constraints/AlignmentConstraint.h>
#include <GUI/Widget.h>

namespace Ice {

namespace GUI {

float HorizontalAlignmentConstraint::getValue() const {
    if (parent() == nullptr) {
        switch (m_align) {
            case HorizontalAlignment::LEFT:
                return -1.0f;
            case HorizontalAlignment::CENTER:
                return 0.0f;
            case HorizontalAlignment::RIGHT:
                return 1.0f;
        }
    }
    
    switch (m_align) {
        case HorizontalAlignment::LEFT:
            return parent()->left(honorPadding());
        case HorizontalAlignment::CENTER:
            return (parent()->left(honorPadding()) + parent()->right(honorPadding())) / 2.0f;
        case HorizontalAlignment::RIGHT:
            return parent()->right(honorPadding());
    }
    
    throw std::runtime_error("Invalid horizontal alignment");
}

float VerticalAlignmentConstraint::getValue() const {
    if (parent() == nullptr) {
        switch (m_align) {
            case VerticalAlignment::BOTTOM:
                return -1.0f;
            case VerticalAlignment::MIDDLE:
                return 0.0f;
            case VerticalAlignment::TOP:
                return 1.0f;
        }
    }
    
    switch (m_align) {
        case VerticalAlignment::BOTTOM:
            return parent()->bottom(honorPadding());
        case VerticalAlignment::MIDDLE:
            return (parent()->bottom(honorPadding()) + parent()->top(honorPadding())) / 2.0f;
        case VerticalAlignment::TOP:
            return parent()->top(honorPadding());
    }
    
    throw std::runtime_error("Invalid vertical alignment");
}

}

}
