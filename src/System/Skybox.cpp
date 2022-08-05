//
//  Skybox.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 18.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <System/Skybox.h>
#include <System/Config.h>
//#include "Controller.h"
#include <glm/gtc/matrix_transform.hpp>
#include <ShadersGL/SkyboxShaderConfigurator.h>

namespace Ice {

Skybox::Skybox() {
   
    m_mesh.vertices().insert(m_mesh.vertices().end(), Right.begin(), Right.end());
    m_mesh.vertices().insert(m_mesh.vertices().end(), Left.begin(), Left.end());
    m_mesh.vertices().insert(m_mesh.vertices().end(), Top.begin(), Top.end());
    m_mesh.vertices().insert(m_mesh.vertices().end(), Bottom.begin(), Bottom.end());
    m_mesh.vertices().insert(m_mesh.vertices().end(), Front.begin(), Front.end());
    m_mesh.vertices().insert(m_mesh.vertices().end(), Back.begin(), Back.end());

    auto& indexBuffer = m_mesh.indices();
    
    for (int nFaceIndex = 0; nFaceIndex < 6; ++nFaceIndex) {
        int nBaseIndex = 4 * nFaceIndex;
        auto arIndices = Indices;
        for (int i = 0; i < Indices.size(); ++i) {
            arIndices[i] = arIndices[i] + nBaseIndex;
        }
        indexBuffer.insert(indexBuffer.end(), arIndices.begin(), arIndices.end());
    }
    //setShaderId("Skybox");
    m_mesh.shaderId() = "Skybox";
}

//void Skybox::setMatrices(const glm::mat4& view, const glm::mat4& projection) {
//    SkyboxShader* pShader = dynamic_cast<SkyboxShader*>(shader());
//    pShader->setProjectionMatrix(projection);
//    auto viewMatrix = view;
//    viewMatrix[3][0] = 0.0f;
//    viewMatrix[3][1] = 0.0f;
//    viewMatrix[3][2] = 0.0f;
//    pShader->setViewMatrix(viewMatrix);
//}

}
