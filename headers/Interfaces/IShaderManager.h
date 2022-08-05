//
//  IShaderManager.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IShaderManager_h
#define IShaderManager_h

#include <string>

class IShaderConfigurator;
class IShaderProgram;

namespace Ice {

class IShaderManager {
public:
    // This method is to be called via the interface by users of the engine
    virtual bool registerShaderProgram(const std::string&, const std::string&, const std::string&, std::unique_ptr<IShaderConfigurator>) noexcept = 0;
    virtual IShaderProgram* getShader(const std::string&) const noexcept = 0;
    virtual ~IShaderManager() {}
};

}

#endif /* IShaderManager_h */
