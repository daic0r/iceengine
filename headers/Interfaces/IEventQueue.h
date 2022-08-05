//
//  IEventQueue.h
//  IceCraft
//
//  Created by Matthias Gruen on 28.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IEventQueue_h
#define IEventQueue_h

namespace Ice {

class Event;

class IEventQueue {
public:
    virtual void queueEvent(Event) noexcept = 0;
    virtual Event* peekEvent() noexcept = 0;
    virtual bool popEvent(Event&) noexcept = 0;

    virtual void queueInternalEvent(Event) noexcept = 0;

    virtual void clearInternalEvents() noexcept = 0;
    virtual void clearUserEvents() noexcept = 0;

    virtual Event* peekInternalEvent() noexcept = 0;
    virtual bool popInternalEvent(Event&) noexcept = 0;
};

}

#endif /* IEventQueue_h */
