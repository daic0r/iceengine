//
//  IShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IShaderConfigurator_h
#define IShaderConfigurator_h

namespace Ice {

struct RenderEnvironment;
class IShaderProgram;

class IShaderConfigurator {
public:
    virtual void setShaderProgram(IShaderProgram*) noexcept = 0;
    virtual void bindAttributes() const noexcept = 0;
    virtual void initialize() noexcept {}
    virtual void getUniformLocations() noexcept {}
	virtual void loadUniforms(const RenderEnvironment&) noexcept {}
    virtual ~IShaderConfigurator() {}
    
};

}

#endif /* IShaderConfigurator_h */
