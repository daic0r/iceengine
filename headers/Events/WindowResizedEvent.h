//
//  WindowResizedEvent.h
//  IceCraft
//
//  Created by Matthias Gruen on 20.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WindowResizedEvent_h
#define WindowResizedEvent_h

#include <Events/Event.h>
#include <System/Types.h>

namespace Ice {

class WindowResizedEventArgs {
    int m_nWidth{ 0 }, m_nHeight{ 0 };

public:
    WindowResizedEventArgs(int w, int h) : m_nWidth{ w }, m_nHeight{ h } {}
    auto width() const noexcept { return m_nWidth; }
    auto height() const noexcept { return m_nHeight; }
};

}

#endif /* WindowResizedEvent_h */
