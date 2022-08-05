//
//  Engine.h
//  CovidSimulator
//
//  Created by Matthias Gruen on 04.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Engine_h
#define Engine_h

#include <IceEngine.h>
#include <memory>
#include <System/Config.h>

namespace Ice {

class ILoader;
class IGame;

class ICEENGINE_API Engine {
    int m_nLastTicks{ -1 };
    std::unique_ptr<IGame> m_pGame;
    
    
public:
    bool init(const Config& config, ILoader* pLoader, std::unique_ptr<IGame>);
    bool run();
    
private:
};

}


#endif /* Engine_h */
