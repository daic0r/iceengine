//
//  ILoader.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ILoader_h
#define ILoader_h

namespace Ice {

class ILoader {
public:
    
    virtual bool loadData() = 0;
    virtual ~ILoader() {}
};

}

#endif /* ILoader_h */
