#pragma once

#include <Interfaces/IShaderConfigurator.h>
#include <glm/vec3.hpp>

namespace Ice {

class SunShaderConfigurator : public IShaderConfigurator {
    IShaderProgram *m_pShaderProgram{ nullptr };
    
    int m_nProjectionMatrixUniformId{ -1 };
    int m_nViewMatrixUniformId{ -1 };
    int m_nPositionUniformId{ -1 };
	int m_nSunColorUniformId{ -1 };
	int m_nViewportUniformId{ -1 };

public:
    void setShaderProgram(IShaderProgram *) noexcept override;
    void bindAttributes() const noexcept override;
    void loadUniforms(const RenderEnvironment &) noexcept override;
    void getUniformLocations() noexcept override;
	void loadPosition(const glm::vec3&) const noexcept;
	void loadSunColor(const glm::vec3&) const noexcept;
	void loadViewportDimensions(int width, int height) const noexcept;
};

}