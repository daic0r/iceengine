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
#include <ranges>
#include <algorithm>
#include <Utils/MeshGeneration/LowPolyTerrainMeshGenerator.h>

namespace Ice {

const std::vector<GLfloat> WaterRendererGL::m_vQuadVertices {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
};

WaterRendererGL::WaterRendererGL() {
    /*
    m_pShaderProgram = RenderToolsGL::createShaderProgram("Water", std::make_unique<WaterShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<WaterShaderConfigurator*>(m_pShaderProgram->configurator());
    m_pShaderProgram->use();
    glCall(m_pShaderConfig->connectTextureUnits());
    m_pShaderProgram->unuse();
    
    m_pFramebuffers = std::make_unique<WaterFramebuffersGL>();
    */

    m_pShaderProgram = std::make_unique<ShaderProgramGL>();
    m_pShaderProgram->fromSource(getVertexShaderSource(), getFragmentShaderSource());
    m_pShaderProgram->use();
    m_nPersViewMatrixID = m_pShaderProgram->getUniformLocation("perspectiveViewMatrix");
    m_nModelMatrixID = m_pShaderProgram->getUniformLocation("modelMatrix");
    m_pShaderProgram->unuse();
   
    //m_pQuad = std::make_unique<RenderObjectGL>(RenderToolsGL::loadVerticesToVAO(m_vQuadVertices, 2));
    //glEnableVertexAttribArray(0);
}

void WaterRendererGL::prepareRendering(const RenderEnvironment& env) noexcept {
    glCall(m_pShaderProgram->use());

    const auto perspectiveViewMatrix = env.projectionMatrix * env.viewMatrix;
    m_pShaderProgram->loadMatrix4f(m_nPersViewMatrixID, perspectiveViewMatrix);
    /*
    m_pShaderConfig->loadUniforms(env);
    m_fMoveFactor += WAVE_SPEED * env.fDeltaTime;
    m_fMoveFactor = fmod(m_fMoveFactor, 1.0f);
    m_pShaderConfig->loadMoveFactor(m_fMoveFactor);
    m_pShaderConfig->loadCameraPosition(env.pCamera->position());
    */
    
    //glCall(glBindVertexArray(m_pQuad->vao()));

    /*
    WaterFramebuffersGL* pFbo = m_pFramebuffers.get();  //reinterpret_cast<WaterFramebuffersGL*>(env.pMiscData);
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
    */
    //glCall(glEnable(GL_BLEND));
    //glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glEnable(GL_DEPTH_TEST);
#ifdef _DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, env.bWireframe ? GL_LINE : GL_FILL);
#endif
}

void WaterRendererGL::render(const RenderEnvironment& env, const std::vector<WaterTile*> &vTiles) noexcept {
    prepareRendering(env);
    for (auto pTile : vTiles) {
        auto& renderObj = std::invoke([](WaterRendererGL* pRend, WaterTile* pTile) -> decltype(auto) {
            auto iter = pRend->m_mTileObjects.find(pTile);
            if (iter != pRend->m_mTileObjects.end())
                return (iter->second);
            return (pRend->registerWaterTile(pTile));
        }, this, pTile);
        glBindVertexArray(renderObj.vao());

        auto modelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3{ pTile->position().x, pTile->height(), pTile->position().y });
        //modelMatrix = glm::scale(modelMatrix, glm::vec3{ pTile->size(), 1.0f, pTile->size() });
        m_pShaderProgram->loadMatrix4f(m_nModelMatrixID, modelMatrix);
        /*
        m_pShaderConfig->loadModelMatrix(modelMatrix);
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vQuadVertices.size()));
        */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderObj.indexBufferAt(0));
        glDrawElements(GL_TRIANGLES, pTile->numTilesH() * pTile->numTilesV() * 2, GL_UNSIGNED_INT, (void*) 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    finishRendering();
}

void WaterRendererGL::finishRendering() noexcept {
    //glCall(glDisable(GL_BLEND));
    glCall(glBindTexture(GL_TEXTURE_2D, 0));
    glCall(glBindVertexArray(0));
    glCall(glDisable(GL_DEPTH_TEST));
#ifdef _DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
    glCall(m_pShaderProgram->unuse());
}

const char* WaterRendererGL::getVertexShaderSource() noexcept {
    return R"(
#version 410

layout(location = 0) in vec3 vertexPos;

uniform mat4 modelMatrix;
uniform mat4 perspectiveViewMatrix;

void main() {
    gl_Position =  perspectiveViewMatrix * modelMatrix * vec4(vertexPos.x, 0, vertexPos.z, 1);
}

)";
}

const char* WaterRendererGL::getFragmentShaderSource() noexcept {
    return R"(
#version 410

out vec4 outColor;

void main() {
    outColor = vec4(0,0,1,1);
}

)";
}

RenderObjectGL& WaterRendererGL::registerWaterTile(WaterTile* pTile) {
    auto iter = m_mTileObjects.find(pTile);
    if (iter != m_mTileObjects.end())
        return iter->second;
    
    MeshGeneration::LowPolyTerrainMeshGenerator<> g{ pTile->numTilesH(), pTile->numTilesV() };// pTile->numTilesH(), pTile->numTilesV(), pTile->tileWidth(), pTile->tileHeight() };
    const auto vMesh = g.generateVertices(pTile->tileWidth(), pTile->tileHeight(), nullptr);
    const auto vIndices = g.generateIndices();

    GLuint nVao;
    glCall(glCreateVertexArrays(1, &nVao));
    glCall(glBindVertexArray(nVao));
    GLuint nVbo;
    
    GLuint buffers[2]; 
    glCall(glCreateBuffers(2, buffers));
    glCall(glNamedBufferStorage(buffers[0], vMesh.size() * sizeof(glm::vec2), &vMesh[0], 0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, buffers[0]));
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*) 0));
    glCall(glEnableVertexAttribArray(0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

    glCall(glNamedBufferStorage(buffers[1], vIndices.size() * sizeof(unsigned int), &vIndices[0], 0));

    glCall(glBindVertexArray(0));
    auto [insert_iter, success ] = m_mTileObjects.emplace(pTile, RenderObjectGL{ nVao });
    insert_iter->second.addIndexBuffer(buffers[1]);
    return insert_iter->second;
}

}

#endif
