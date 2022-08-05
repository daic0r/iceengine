//
//  FinalAction.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Utils/FinalAction.h>

namespace Ice {

FinalAction::FinalAction(const std::function<void()>& cleanUp) {
    m_cleanUp = cleanUp;
}

FinalAction::FinalAction(FinalAction&& other) {
    *this = std::move(other);
}

FinalAction& FinalAction::operator=(FinalAction &&other) {
    m_bDirty = true;
    other.m_bDirty = false;
    m_cleanUp = other.m_cleanUp;
    return *this;
}

FinalAction::~FinalAction() {
    if (m_bDirty)
        m_cleanUp();
}

FinalAction finally(const std::function<void()>& func) {
    return FinalAction{ func };
}

}
