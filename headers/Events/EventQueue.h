//
//  EventQueue.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef EventQueue_h
#define EventQueue_h

#include <deque>
#include "Event.h"
#include <mutex>
#include <unordered_map>
#include <Interfaces/IEventQueue.h>

namespace Ice {

class EventQueue : public IEventQueue {
    mutable std::recursive_mutex m_internalMtx;
    mutable std::recursive_mutex m_userMtx;
    mutable std::recursive_mutex m_listenerMtx;
    
    std::deque<Event> m_vInternalEvents;
    std::deque<Event> m_vUserEvents;
    
    std::deque<Event>::iterator m_userEventIter;
    std::deque<Event>::iterator m_internalEventIter;
    bool m_bIteratingUserEvent{ false };
    bool m_bIteratingInternalEvents{ false };
    
    static Event* _peekEvent(decltype(m_userEventIter)&, decltype(m_vUserEvents)&, bool&);
    static bool _popEvent(decltype(m_vUserEvents)&, Event&);
    
public:
    EventQueue() noexcept;

    void queueInternalEvent(Event) noexcept override;
    
    void clearInternalEvents() noexcept override;
    void clearUserEvents() noexcept override;
    
    void queueEvent(Event) noexcept override;

    Event* peekEvent() noexcept override;
    bool popEvent(Event&) noexcept override;
    
    Event* peekInternalEvent() noexcept override;
    bool popInternalEvent(Event&) noexcept override;
};

}

#endif /* EventQueue_h */
