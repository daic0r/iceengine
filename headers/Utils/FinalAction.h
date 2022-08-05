//
//  FinalAction.h
//  IceCraft
//
//  Created by Matthias Gruen on 16.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef FinalAction_h
#define FinalAction_h

#include <functional>

namespace Ice {

class FinalAction {

    bool m_bDirty{ true };
    std::function<void()> m_cleanUp;
    
public:
    FinalAction(const std::function<void()>&);
    FinalAction(const FinalAction&) = delete;
    FinalAction& operator=(const FinalAction&) = delete;
    FinalAction(FinalAction&&);
    FinalAction& operator=(FinalAction&&);
    
    ~FinalAction();
};

FinalAction finally(const std::function<void()>& func);

}

#endif /* FinalAction_h */
