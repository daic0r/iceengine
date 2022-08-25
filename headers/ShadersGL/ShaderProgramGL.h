//
//  ShaderProgramGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 07.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef Shader_h
#define Shader_h

#include <string>
#include <tuple>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <memory>

#include "../Interfaces/IShaderProgram.h"

namespace Ice {

struct RenderEnvironment;
class IShaderConfigurator;

class ShaderProgramGL : public IShaderProgram {
    unsigned int m_programId = 0;
    unsigned int m_vertexShaderId = 0;
    unsigned int m_fragmentShaderId = 0;
    
    std::string m_strVertexShaderPath;
    std::string m_strFragmentShaderPath;
    
    //std::unique_ptr<IShaderConfigurator> m_pConfigurator;
    IShaderConfigurator* m_pConfigurator{};
    
protected:
    static unsigned int loadFromFile(const std::string&, unsigned int);
    static std::string getLog(bool, unsigned int);
    void primeShaders();
    static unsigned int loadFromSource(std::string_view, unsigned int);
    
public:

    //ShaderProgram(const std::string&, const std::string&);
    ShaderProgramGL() = default; 
    ShaderProgramGL(const std::string&, const std::string&, std::unique_ptr<IShaderConfigurator>);
    ShaderProgramGL(const ShaderProgramGL&) = delete;
    ShaderProgramGL& operator=(const ShaderProgramGL&) = delete;
    ShaderProgramGL(ShaderProgramGL&&);
    ShaderProgramGL& operator=(ShaderProgramGL&&);
    virtual ~ShaderProgramGL();

    inline unsigned int id() const noexcept { return m_programId; }
    inline IShaderConfigurator* configurator() const noexcept override { return m_pConfigurator; } //.get(); }
    void setConfigurator(std::unique_ptr<IShaderConfigurator> pConfig) noexcept override;
    
//    void bindAttributes(IShaderProgram*) const noexcept = 0;
//    void getUniformLocations(IShaderProgram*) noexcept {}
//    void loadUniforms(IShaderProgram*, const RenderEnvironment&) noexcept = 0;
    void load() override;
    void fromSource(std::string_view, std::string_view) override;
    void use() override;
    void unuse() override;

    int getUniformLocation(const std::string& strName) const noexcept override;
    void bindAttribute(int nIndex, const std::string& strName) const noexcept override;
    void loadMatrix4f(int uniformID, const glm::mat4&) const noexcept override;
    void loadVector2f(int uniformID, const glm::vec2&) const noexcept override;
    void loadFloat(int uniformID, float) const noexcept override;
    void loadVector3f(int uniformID, const glm::vec3&) const noexcept override;
    void loadVector4f(int uniformID, const glm::vec4&) const noexcept override;
    void loadInt(int uniformID, int) const noexcept override;

    static const char* canvasVertexShader() noexcept;
    static const char* canvasFragmentShader() noexcept;
};

}

#endif /* Shader_h */
