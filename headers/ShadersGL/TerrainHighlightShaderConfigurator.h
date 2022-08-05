//
//  TerrainHighlightShaderConfigurator.h
//  IceCraft
//
//  Created by Matthias Gruen on 26.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TerrainHighlightShaderConfigurator_h
#define TerrainHighlightShaderConfigurator_h

#include <Interfaces/IShaderConfigurator.h>
#include <glm/mat4x4.hpp>

namespace Ice {

class TerrainHighlightShaderConfigurator : public IShaderConfigurator {
    IShaderProgram *m_pShaderProgram{ nullptr };
    
    int m_nProjectionMatrixUniformId{ -1 };
    int m_nViewMatrixUniformId{ -1 };
    int m_nModelMatrixUniformId{ -1 };

public:
    void setShaderProgram(IShaderProgram *) noexcept override;
    void bindAttributes() const noexcept override;
    void loadUniforms(const RenderEnvironment &) noexcept override;
    void getUniformLocations() noexcept override;
    void loadModelMatrix(const glm::mat4&) const noexcept;
};

}

#endif /* TerrainHighlightShaderConfigurator_h */
