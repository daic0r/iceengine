//
//  ShaderProgramGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 07.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <fstream>
#include <SDL2/SDL_opengl.h>
#include <vector>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Interfaces/IShaderConfigurator.h>

namespace Ice {

//ShaderProgram::ShaderProgram(const std::string& strPathVertexShader, const std::string& strPathFragmentShader) {
//    m_strVertexShaderPath = strPathVertexShader;
//    m_strFragmentShaderPath = strPathFragmentShader;
//}

ShaderProgramGL::ShaderProgramGL(const std::string& strPathVertexShader, const std::string& strPathFragmentShader, std::unique_ptr<IShaderConfigurator> pConfig)
//: ShaderProgram{ strPathVertexShader, strPathFragmentShader } {
{
    m_strVertexShaderPath = strPathVertexShader;
    m_strFragmentShaderPath = strPathFragmentShader;
    if (pConfig)
        setConfigurator(std::move(pConfig));
}

void ShaderProgramGL::load() {
    m_vertexShaderId = loadFromFile(m_strVertexShaderPath, GL_VERTEX_SHADER);
    m_fragmentShaderId = loadFromFile(m_strFragmentShaderPath, GL_FRAGMENT_SHADER);
    primeShaders();
}

void ShaderProgramGL::fromSource(std::string_view strVertexShader, std::string_view strFragmentShader) {
    m_vertexShaderId = loadFromSource(strVertexShader, GL_VERTEX_SHADER);
    m_fragmentShaderId = loadFromSource(strFragmentShader, GL_FRAGMENT_SHADER);
    primeShaders();
}

ShaderProgramGL& ShaderProgramGL::operator=(ShaderProgramGL&& other) {
    m_vertexShaderId = other.m_vertexShaderId;
    m_fragmentShaderId = other.m_fragmentShaderId;
    m_programId = other.m_programId;
    m_pConfigurator = std::move(other.m_pConfigurator);
    other.m_vertexShaderId = other.m_fragmentShaderId = other.m_programId = 0;
    return *this;
}

ShaderProgramGL::ShaderProgramGL(ShaderProgramGL&& other) {
    *this = std::move(other);
}

ShaderProgramGL::~ShaderProgramGL() {
    unuse();
    delete m_pConfigurator;
    if (m_vertexShaderId != 0) {
        glDetachShader(m_programId, m_vertexShaderId);
        glDeleteShader(m_vertexShaderId);
    }
    if (m_fragmentShaderId != 0) {
        glDetachShader(m_programId, m_fragmentShaderId);
        glDeleteShader(m_fragmentShaderId);
    }
    if (m_programId != 0)
        glDeleteProgram(m_programId);
}

void ShaderProgramGL::setConfigurator(std::unique_ptr<IShaderConfigurator> pConfig) noexcept {
    pConfig->setShaderProgram(this);
    m_pConfigurator = pConfig.release(); // std::move(pConfig);
}

unsigned int ShaderProgramGL::loadFromFile(const std::string& filename, unsigned int shaderType) {
    std::ifstream f;
    
    f.open(filename);
    if (!f.good()) {
        throw std::runtime_error("Cannot load shader file");
    }
    
    std::string strContents;
    std::string strLine;
    while (std::getline(f, strLine)) {
        strContents += strLine;
        strContents += "\n";
    }
    f.close();
   
    return loadFromSource(strContents, shaderType);
}

std::string ShaderProgramGL::getLog(bool bProgramOrShader, unsigned int shader )
{
    //Make sure name is shader
    if( glIsShader( shader ) )
    {
        //Shader log length
        int infoLogLength = 0;
        int maxLength = infoLogLength;
        
        //Get info string length
        if (!bProgramOrShader)
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
        else
            glGetProgramiv( shader, GL_INFO_LOG_LENGTH, &maxLength );
        
        //Allocate string
        auto infoLog = std::make_unique<char[]>(maxLength);
        
        //Get info log
        if (!bProgramOrShader)
            glGetShaderInfoLog( shader, maxLength, &infoLogLength, infoLog.get() );
        else
            glGetProgramInfoLog( shader, maxLength, &infoLogLength, infoLog.get() );
        
        if( infoLogLength > 0 )
        {
            return infoLog.get();
        }
    }

    std::string strErr;
    strErr.append("%d is not a shader/program", shader);
    throw std::runtime_error(strErr);
}

void ShaderProgramGL::primeShaders() {
    const auto pConfig = configurator();
    if (pConfig)
        pConfig->initialize();
    
    m_programId = glCreateProgram();
    
    //m_vertexShaderId = loadFromFile(strPathVertexShader, GL_VERTEX_SHADER);
    glAttachShader(m_programId, m_vertexShaderId);
    //m_fragmentShaderId = loadFromFile(strPathFragmentShader, GL_FRAGMENT_SHADER);
    glAttachShader(m_programId, m_fragmentShaderId);
    if (pConfig)
        pConfig->bindAttributes();
    glLinkProgram(m_programId);
    
    int programSuccess = GL_TRUE;
    glGetProgramiv( m_programId, GL_LINK_STATUS, &programSuccess );
    if (programSuccess != GL_TRUE) {
        throw std::runtime_error(getLog(true, m_programId));
    }

    if (pConfig)
        pConfig->getUniformLocations();
}

unsigned int ShaderProgramGL::loadFromSource(std::string_view strSource, unsigned int nShaderType) {
    const auto shaderId = glCreateShader(nShaderType);
    
    const GLchar* source = (const GLchar*) strSource.data();
    glShaderSource(shaderId, 1, &source, nullptr);
    
    glCompileShader(shaderId);
    int bShaderCompiled;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &bShaderCompiled);
    if (bShaderCompiled != GL_TRUE) {
        throw std::runtime_error(getLog(false, shaderId));
    }
    return shaderId;
}

void ShaderProgramGL::use() {
    glUseProgram(id());
}


void ShaderProgramGL::unuse() {
    glUseProgram(0);
}

void ShaderProgramGL::bindAttribute(int nIndex, const std::string& strName) const noexcept {
    glBindAttribLocation(m_programId, nIndex, strName.c_str());
}

int ShaderProgramGL::getUniformLocation(const std::string& strName) const noexcept {
    int nRet = glGetUniformLocation(m_programId, strName.c_str());
//    if (nRet == -1) {
//        throw std::runtime_error("Could not get uniform location");
//    }
    return nRet;
}

void ShaderProgramGL::loadMatrix4f(int uniformID, const glm::mat4 &mat) const noexcept {
    glUniformMatrix4fv(uniformID, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgramGL::loadVector3f(int uniformID, const glm::vec3 &vec) const noexcept {
    glUniform3fv(uniformID, 1, glm::value_ptr(vec));
}

void ShaderProgramGL::loadVector4f(int uniformID, const glm::vec4 &vec) const noexcept {
    glUniform4fv(uniformID, 1, glm::value_ptr(vec));
}

void ShaderProgramGL::loadVector2f(int uniformID, const glm::vec2& vec) const noexcept {
    glUniform2fv(uniformID, 1, glm::value_ptr(vec));
}

void ShaderProgramGL::loadFloat(int uniformID, float f) const noexcept {
    glUniform1f(uniformID, f);
}

void ShaderProgramGL::loadInt(int uniformID, int i) const noexcept {
    glUniform1i(uniformID, i);
}

const char* ShaderProgramGL::canvasVertexShader() noexcept {
    return R"(
#version 410

layout (location = 0) in vec2 position;
out vec2 texCoord;

void main() {
    texCoord = position * 0.5 + 0.5;
    gl_Position = vec4(position, 0.0, 1.0);
}
)";
}

const char* ShaderProgramGL::canvasFragmentShader() noexcept {
    return R"(
#version 410

in vec2 texCoord;
uniform sampler2D tex;

layout (location = 0) out vec4 outColor;

void main() {
    outColor = texture(tex, texCoord);
}
)";
}

}
