//
//  EventId.h
//  IceCraft
//
//  Created by Matthias Gruen on 28.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef EventId_h
#define EventId_h

namespace Ice {

enum class EventId {
    TERRAIN_TILE_SELECTED_EVENT,
    MOUSE_MOTION_EVENT,
    MOUSE_BUTTON_EVENT,
    TEXT_INPUT_EVENT,
    WINDOW_RESIZED_EVENT,
    KEY_DOWN_EVENT,
    MOUSE_WHEEL_EVENT,
    NO_EVENT
};

}

#endif /* EventId_h */
