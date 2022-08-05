//
//  InputReceiverComponent.h
//  IceCraft
//
//  Created by Matthias Gruen on 04.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef InputReceiverComponent_h
#define InputReceiverComponent_h

#include <SDL2/SDL.h>
#include <Components/ComponentIds.h>
#include <functional>
#include <Entities/Entity.h>
#include <vector>

namespace Ice {

struct InputReceiverComponent {
    static constexpr EntityComponentId id() noexcept { return INPUT_RECEIVER_COMPONENT; }
    
    std::vector<std::function<bool(Entity, std::uint32_t, SDL_MouseMotionEvent*, SDL_MouseButtonEvent*, SDL_MouseWheelEvent*)>> m_vMouseHandlers;
    std::vector<std::function<bool(Entity, std::uint32_t, SDL_KeyboardEvent*)>> m_vKeyboardHandlers;
};

}

#endif /* InputReceiverComponent_h */
