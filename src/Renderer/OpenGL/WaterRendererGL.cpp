//
//  WaterRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifdef RENDERER_OPEN_GL

#include <GL/glew.h>
#include <Renderer/OpenGL/WaterRendererGL.h>
#include <Interfaces/IShaderConfigurator.h>
#include <ShadersGL/WaterShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <System/Config.h>
#include <System/SystemServices.h>
#include <Interfaces/IShaderManager.h>
#include <Interfaces/IShaderProgram.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>
#include <Renderer/RenderEnvironment.h>
#include <glm/gtc/matrix_transform.hpp>
#include <World/WaterTile.h>
#include <glm/mat4x4.hpp>
//#include <System/Controller.h>
#include <Renderer/OpenGL/WaterFramebuffersGL.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <System/Camera.h>

namespace Ice {

const std::vector<GLfloat> WaterRendererGL::m_vQuadVertices {
    -1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, -1.0f,
    1.0f, 1.0f
};

WaterRendererGL::WaterRendererGL() {
//    std::unique_ptr<IShaderConfigurator> pConfig = std::make_unique<WaterShaderConfigurator>();
//
//    std::string vertexShaderPath{ Config::BasePath };
//    vertexShaderPath.append("ShadersGL/WaterVertexShader.glsl");
//    std::string fragmentShaderPath{ Config::BasePath };
//    fragmentShaderPath.append("ShadersGL/WaterFragmentShader.glsl");
//
//    shaderManager->registerShaderProgram("waterShader", vertexShaderPath, fragmentShaderPath, std::move(pConfig));
//    m_pShaderProgram = shaderManager->getShader("waterShader");
//    m_pShaderConfig = dynamic_cast<WaterShaderConfigurator*>(m_pShaderProgram->configurator());
    m_pShaderProgram = RenderToolsGL::createShaderProgram("Water", std::make_unique<WaterShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<WaterShaderConfigurator*>(m_pShaderProgram->configurator());
    m_pShaderProgram->use();
    glCall(m_pShaderConfig->connectTextureUnits());
    m_pShaderProgram->unuse();
    
//    std::string texturePath { Config::BasePath };
//    texturePath.append("/Assets/waterDUDV.png");
    //textureManager->loadTextureAtlas("waterDuDv", texturePath);
    //m_pDuDvTexture = dynamic_cast<TextureGL*>(textureManager->getTextureAtlas("waterDuDv"));
    
//    texturePath = Config::BasePath;
//    texturePath.append("/Assets/waterNormal.png");
    //textureManager->loadTextureAtlas("waterNormal", texturePath);
    //m_pNormalTexture = dynamic_cast<TextureGL*>(textureManager->getTextureAtlas("waterNormal"));
    
    
    m_pQuad = std::make_unique<RenderObjectGL>(RenderToolsGL::loadVerticesToVAO(m_vQuadVertices, 2));
}

void WaterRendererGL::prepareRendering(const RenderEnvironment& env) noexcept {
    glCall(m_pShaderProgram->use());

    m_pShaderConfig->loadUniforms(env);
    m_fMoveFactor += WAVE_SPEED * env.fDeltaTime;
    m_fMoveFactor = fmod(m_fMoveFactor, 1.0f);
    m_pShaderConfig->loadMoveFactor(m_fMoveFactor);
    m_pShaderConfig->loadCameraPosition(env.pCamera->position());
    
    glCall(glBindVertexArray(m_pQuad->vao()));
    glCall(glEnableVertexAttribArray(0));

    WaterFramebuffersGL* pFbo = reinterpret_cast<WaterFramebuffersGL*>(env.pMiscData);
    glCall(glActiveTexture(GL_TEXTURE0));
    glCall(glBindTexture(GL_TEXTURE_2D, pFbo->reflectionTexture()));
    glCall(glActiveTexture(GL_TEXTURE1));
    glCall(glBindTexture(GL_TEXTURE_2D, pFbo->refractionTexture()));
    glCall(glActiveTexture(GL_TEXTURE2));
    glCall(glBindTexture(GL_TEXTURE_2D, m_pDuDvTexture->textureId()));
    glCall(glActiveTexture(GL_TEXTURE3));
    glCall(glBindTexture(GL_TEXTURE_2D, m_pNormalTexture->textureId()));
    glCall(glActiveTexture(GL_TEXTURE4));
    glCall(glBindTexture(GL_TEXTURE_2D, pFbo->refractionDepthTexture()));
    
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void WaterRendererGL::render(const RenderEnvironment& env, const std::vector<WaterTile*> &vTiles) noexcept {
    prepareRendering(env);
    for (auto pTile : vTiles) {
        auto modelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3{ pTile->position().x, pTile->height(), pTile->position().y });
        modelMatrix = glm::scale(modelMatrix, glm::vec3{ pTile->size(), 1.0f, pTile->size() });
        m_pShaderConfig->loadModelMatrix(modelMatrix);
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vQuadVertices.size()));
    }
    finishRendering();
}

void WaterRendererGL::finishRendering() noexcept {
    glCall(glDisable(GL_BLEND));
    glCall(glBindTexture(GL_TEXTURE_2D, 0));
    glCall(glDisableVertexAttribArray(0));
    glCall(glBindVertexArray(0));
    //glCall(glDisable(GL_DEPTH_TEST));
    glCall(m_pShaderProgram->unuse());
}

}

#endif
