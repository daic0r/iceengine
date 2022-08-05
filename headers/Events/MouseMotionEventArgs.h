//
//  MouseMotionEventArgs.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MouseMotionEventArgs_h
#define MouseMotionEventArgs_h

#include <SDL2/SDL.h>
#include <Events/Event.h>
#include <System/Types.h>

namespace Ice {

class MouseMotionEventArgs {
    
    SDL_MouseMotionEvent m_event;
    
public:
    MouseMotionEventArgs(const SDL_MouseMotionEvent& event) : m_event{ event } {}
    const auto& event() const noexcept { return m_event; }
    
    int posX() const noexcept { return m_event.x; }
    int posY() const noexcept { return m_event.y; }
    int offsetX() const noexcept { return m_event.xrel; }
    int offsetY() const noexcept { return m_event.yrel; }
    std::uint8_t buttonState() const noexcept {
        std::uint8_t ret{ 0 };
        if (m_event.state & SDL_BUTTON_LMASK)
            ret |= static_cast<std::uint8_t>(MouseButton::LEFT);
        if (m_event.state & SDL_BUTTON_MMASK)
            ret |= static_cast<std::uint8_t>(MouseButton::MIDDLE);
        if (m_event.state & SDL_BUTTON_RMASK)
            ret |= static_cast<std::uint8_t>(MouseButton::RIGHT);
        if (ret == 0)
            ret = static_cast<std::uint8_t>(MouseButton::NONE);
        return ret;
    }
};

}


#endif /* MouseMotionEventArgs_h */
