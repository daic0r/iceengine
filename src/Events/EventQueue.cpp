//
//  EventQueue.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 27.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <Events/EventQueue.h>

namespace Ice {

EventQueue::EventQueue() noexcept {
    m_userEventIter = m_vUserEvents.end();
}

void EventQueue::queueInternalEvent(Event pEvent) noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_internalMtx };
    m_vInternalEvents.emplace_back(std::move(pEvent));
}

void EventQueue::queueEvent(Event pEvent) noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_userMtx };
    m_vUserEvents.emplace_back(std::move(pEvent));
}

void EventQueue::clearInternalEvents() noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_internalMtx };
    m_vInternalEvents.clear();
    m_internalEventIter = m_vInternalEvents.end();
    m_bIteratingInternalEvents = false;
}

void EventQueue::clearUserEvents() noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_userMtx };
    m_vUserEvents.clear();
    m_userEventIter = m_vUserEvents.end();
    m_bIteratingUserEvent = false;
}

Event* EventQueue::_peekEvent(decltype(m_userEventIter)& iter, decltype(m_vUserEvents)& events, bool& bIsIterating) {
    if (!bIsIterating) {
        iter = events.size() > 0 ? events.begin() : events.end();
        bIsIterating = true;
    }
    if (iter == events.end()) {
        bIsIterating = false;
        return nullptr;
    }
    return std::addressof(*(iter++));
}

Event* EventQueue::peekEvent() noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_userMtx };
    return _peekEvent(m_userEventIter, m_vUserEvents, m_bIteratingUserEvent);
}

Event* EventQueue::peekInternalEvent() noexcept {
    std::lock_guard<std::recursive_mutex> guard { m_internalMtx };
    return _peekEvent(m_internalEventIter, m_vInternalEvents, m_bIteratingInternalEvents);
}

bool EventQueue::_popEvent(decltype(m_vUserEvents)& events, Event& ret) {
    if (events.size() > 0) {
        ret = std::move(events.front());
        events.pop_front();
        return true;
    }
    return false;
}


bool EventQueue::popEvent(Event& ret) noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_userMtx };
    auto bRet = _popEvent(m_vUserEvents, ret);
    if (bRet)
        m_userEventIter = m_vUserEvents.begin();
    return bRet;
}

bool EventQueue::popInternalEvent(Event& ret) noexcept {
    std::lock_guard<std::recursive_mutex> guard{ m_internalMtx };
    auto bRet = _popEvent(m_vInternalEvents, ret);
    if (bRet)
        m_internalEventIter = m_vInternalEvents.begin();
    return bRet;
}

}
