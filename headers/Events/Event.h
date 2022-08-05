//
//  Event.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Event_h
#define Event_h

#include <memory>
#include <any>
#include "EventId.h"

namespace Ice {

class IEventArgs {
public:
    virtual ~IEventArgs() {};
};

class Event {
    EventId m_id{ EventId::NO_EVENT };
    std::any m_args;

public:
    Event() = default;
    Event(EventId id, std::any args) noexcept : m_id{ id }, m_args{ std::move(args) } {}
    
    auto id() const noexcept { return m_id; }
    auto& args() noexcept { return m_args; }

    void setArgs(std::any args) noexcept { m_args = std::move(args); }
};

}

#endif /* Event_h */
