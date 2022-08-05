#pragma once

#include <SDL2/SDL_opengl.h>
#include <Interfaces/IShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <string>
#include <glm/mat4x4.hpp>

namespace Ice {

class ShaderProgramGL;

class ShadowShaderConfigurator : public IShaderConfigurator {
	GLuint m_nCommonMatricesUBOIndex{ 0 };

	ShaderProgramGL* m_pShaderProgram{ nullptr };

public:
	void setShaderProgram(IShaderProgram* pProgram) noexcept override { m_pShaderProgram = dynamic_cast<ShaderProgramGL*>(pProgram); }
    void getUniformLocations() noexcept override;
	void bindAttributes() const noexcept override;

};

}