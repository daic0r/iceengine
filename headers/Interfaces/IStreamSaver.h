//
//  IStreamSaver.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IStreamSaveable_h
#define IStreamSaveable_h

#include <iostream>
#include <Entities/Entity.h>

namespace Ice {

class IStreamSaver {
public:
    virtual void save(std::ostream&, Entity) const = 0;
};

}

#endif /* IStreamSaveable_h */
