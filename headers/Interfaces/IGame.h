//
//  IGame.h
//  IceCraft
//
//  Created by Matthias Gruen on 27.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IGame_h
#define IGame_h

namespace Ice {

class IEventQueue;

class IGame {
public:
    
    virtual void update(float fDeltaTime, IEventQueue*) noexcept = 0;
    virtual void init() = 0;
    virtual ~IGame() {}
};

}

#endif /* IGame_h */
