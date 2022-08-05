//
//  MouseButtonEventArgs.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef MouseButtonEventArgs_h
#define MouseButtonEventArgs_h

#include <SDL2/SDL.h>
#include <Events/Event.h>
#include <System/Types.h>

namespace Ice {

class MouseButtonEventArgs {
    SDL_MouseButtonEvent m_event;
    
public:
    
    MouseButtonEventArgs(const SDL_MouseButtonEvent& event) : m_event{ event } {}
    auto button() const noexcept {
        switch (m_event.button) {
            case SDL_BUTTON_LEFT:
                return MouseButton::LEFT;
            case SDL_BUTTON_MIDDLE:
                return MouseButton::MIDDLE;
            case SDL_BUTTON_RIGHT:
                return MouseButton::RIGHT;
        }
        return MouseButton::NONE;
    }
    bool pressed() const noexcept {
        switch (m_event.state) {
            case SDL_PRESSED:
                return true;
            case SDL_RELEASED:
                return false;
        }
        return false;
    }
    bool doubleClick() const noexcept {
        if (m_event.clicks == 1)
            return false;
        return true;
    }
    int posX() const noexcept { return m_event.x; }
    int posY() const noexcept { return m_event.y; }
    
};

}

#endif /* MouseButtonEventArgs_h */
