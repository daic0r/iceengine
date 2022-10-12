//
//  IUIConstraint.h
//  IceCraft
//
//  Created by Matthias Gruen on 15.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IUIConstraint_h
#define IUIConstraint_h

#include <IceEngine.h>

namespace Ice {

namespace GUI {

class IUIConstraint {
public:
    virtual bool dynamic() const noexcept = 0;
    virtual float getValue() const = 0;
    virtual ~IUIConstraint() = default;
};

}

}

#endif /* IUIConstraint_h */
