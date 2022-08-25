//
//  IShaderProgram.h
//  IceCraft
//
//  Created by Matthias Gruen on 09.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef IShaderProgram_h
#define IShaderProgram_h

#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <string_view>
#include <memory>

namespace Ice {

class IShaderConfigurator;

class IShaderProgram {
public:
    virtual IShaderConfigurator* configurator() const noexcept = 0;
    virtual void setConfigurator(std::unique_ptr<IShaderConfigurator> pConfig) noexcept = 0;
    
    virtual void use() = 0;
    virtual void unuse() = 0;
    virtual void load() = 0;
    virtual void fromSource(std::string_view, std::string_view) = 0;
    virtual int getUniformLocation(const std::string& strName) const noexcept = 0;
    virtual void bindAttribute(int nIndex, const std::string& strName) const noexcept = 0;
    virtual void loadMatrix4f(int uniformID, const glm::mat4&) const noexcept = 0;
    virtual void loadVector2f(int uniformID, const glm::vec2&) const noexcept = 0;
    virtual void loadVector3f(int uniformID, const glm::vec3&) const noexcept = 0;
    virtual void loadVector4f(int uniformID, const glm::vec4&) const noexcept = 0;
    virtual void loadFloat(int uniformID, float) const noexcept = 0;
    virtual void loadInt(int uniformID, int) const noexcept = 0;
    virtual ~IShaderProgram() = default;
};

}

#endif /* IShaderProgram_h */
