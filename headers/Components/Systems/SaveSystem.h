//
//  SaveSystem.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SaveSystem_h
#define SaveSystem_h

#include <IceEngine.h>
#include <iostream>
#include <Entities/EntityComponentSystem.h>
#include <Components/SaveComponent.h>

namespace Ice {

class SaveSystem : public EntityComponentSystem<SaveComponent> {
   
public:
    template<typename Output>
    void save(Output&);

    template<typename Output>
    void save(Output&, Entity);
};

}

#endif /* SaveSystem_h */
