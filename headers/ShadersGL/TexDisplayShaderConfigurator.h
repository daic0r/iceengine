#pragma once

#include <Interfaces/IShaderConfigurator.h>
#include <SDL2/SDL_opengl.h>

namespace Ice {

class IShaderProgram;

class TexDisplayShaderConfigurator : public IShaderConfigurator {

	IShaderProgram* m_pShaderProgram{ nullptr };
	GLint m_nTexUniformId{ -1 };

public:
	void setShaderProgram(IShaderProgram* pProgram) noexcept override { m_pShaderProgram = pProgram; }
	void bindAttributes() const noexcept override;
	void getUniformLocations() noexcept override;
	void loadUniforms(const RenderEnvironment&) noexcept override;

};

}