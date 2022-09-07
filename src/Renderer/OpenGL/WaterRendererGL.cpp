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
#include <Interfaces/IModelRenderer.h>
#include <Interfaces/ITerrainRenderer.h>
#include <Interfaces/IPostProcessingEffect.h>
#include <glm/gtc/matrix_transform.hpp>

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
    m_nReflectionTextureID = m_pShaderProgram->getUniformLocation("reflectionTexture");
    m_nRefractionTextureID = m_pShaderProgram->getUniformLocation("refractionTexture");
    m_nRefractionDepthTextureID = m_pShaderProgram->getUniformLocation("refractionDepthTexture");
    m_nWaterLevelID = m_pShaderProgram->getUniformLocation("waterLevel");
    m_nCameraPosID = m_pShaderProgram->getUniformLocation("cameraPos");
    m_nDistPlanesID = m_pShaderProgram->getUniformLocation("distPlanes");
    m_nTimeID = m_pShaderProgram->getUniformLocation("time");
    m_nGridSizeID = m_pShaderProgram->getUniformLocation("gridSize");
    m_pShaderProgram->loadInt(m_nReflectionTextureID, 0);
    m_pShaderProgram->loadInt(m_nRefractionTextureID, 1);
    m_pShaderProgram->loadInt(m_nRefractionDepthTextureID, 2);
    m_pShaderProgram->unuse();
   
    m_pQuad = std::make_unique<RenderObjectGL>(RenderToolsGL::loadVerticesToVAO(m_vQuadVertices, 2));
    glEnableVertexAttribArray(0);

    m_pPreviewShader = std::make_unique<ShaderProgramGL>();
    m_pPreviewShader->fromSource(R"(
#version 410

in vec2 vertexPos;
out vec2 texCoord;

void main() {
    gl_Position = vec4(0.25 * vertexPos.x, 0.25 * vertexPos.y, 0.0, 1.0) * 2.0 - 1.0;
    texCoord = vec2(vertexPos.x, vertexPos.y);
}
    )",
    R"(
#version 410

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D tex;
uniform vec2 distPlanes; // near and far

const float realnear = 1.0f;
const float realfar = 1500.0f;

float LinearizeDepth(float depth) 
{
    float near = realnear;
    float far = realfar;
    float z = depth * 2.0 - 1.0; // back to NDC 
    float result = (2.0 * near * far) / (far + near - z * (far - near));
    return result;
}

float toLinearDepth(float depth) 
{
    float distNearPlane = distPlanes.x;
    float distFarPlane = distPlanes.y;
    float z = depth * 2.0 - 1.0; // back to NDC 
    float frustumDepth = distFarPlane - distNearPlane;
    float result = (2.0 * distNearPlane * distFarPlane) / (distFarPlane + distNearPlane - z * (frustumDepth));
    return (result - distNearPlane) / (frustumDepth);
}
void main() {
    float c = (toLinearDepth(texture(tex, texCoord).r)); // - distPlanes.x) / (distPlanes.y - distPlanes.x);
    outColor = vec4(c,c,c,1);
}
    )"
    );

    m_pGraphicsSystem = systemServices.getGraphicsSystem();

    GLint nTexLoc = m_pPreviewShader->getUniformLocation("tex");
    GLint nDistPlanes = m_pPreviewShader->getUniformLocation("distPlanes");
    m_pPreviewShader->use();
    m_pPreviewShader->loadInt(nTexLoc, 0);
    m_pPreviewShader->loadVector2f(nDistPlanes, glm::vec2{ m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distFarPlane() });
    m_pPreviewShader->unuse();

    m_pModelRenderer = entityManager.getSystem<ObjectRenderingSystem, true>();
    m_pTerrainRenderer = entityManager.getSystem<TerrainRenderingSystem, false>();
}

void WaterRendererGL::setWaterLevel(float f) noexcept {
    m_fWaterLevel = f;
    m_pShaderProgram->use();
    m_pShaderProgram->loadFloat(m_nWaterLevelID, m_fWaterLevel);
    m_pShaderProgram->unuse();
}

void WaterRendererGL::setGridSize(float f) noexcept {
    m_pShaderProgram->use();
    m_pShaderProgram->loadFloat(m_nGridSizeID, f);
    m_pShaderProgram->unuse();
}

void WaterRendererGL::prepareRendering(const RenderEnvironment& env) noexcept {

    glCall(m_pShaderProgram->use());

    const auto perspectiveViewMatrix = env.projectionMatrix * env.viewMatrix;
    m_pShaderProgram->loadMatrix4f(m_nPersViewMatrixID, perspectiveViewMatrix);
    m_pShaderProgram->loadVector3f(m_nCameraPosID, env.pCamera->position());
    m_pShaderProgram->loadVector2f(m_nDistPlanesID, glm::vec2{ m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distFarPlane() });
    m_pShaderProgram->loadFloat(m_nTimeID, m_fWaveTime);
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
    glCall(glEnable(GL_BLEND));
    //glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#ifdef _DEBUG
    glPolygonMode(GL_FRONT_AND_BACK, env.bWireframe ? GL_LINE : GL_FILL);
#endif
}

void WaterRendererGL::render(const RenderEnvironment& env, const std::vector<WaterTile*> &vTiles) noexcept {
    GLint nLastFBO{-1};
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &nLastFBO);

    auto myEnv = env;
    Camera cam = *myEnv.pCamera;
    cam.setHeightGetterFunc(nullptr);

    myEnv.fWaterLevel = m_fWaterLevel + 0.4f;
    const auto t = (myEnv.fWaterLevel.value() - cam.position().y) / cam.direction().y;
    cam.m_lookAt.force(cam.position() + t * cam.direction());
    cam.m_fDistance.force(glm::length(cam.lookAt() - cam.position()));
    cam.m_fPitch.force(-cam.pitch());
    cam.update(0.0);

    myEnv.bMainRenderPass = false;
    myEnv.pCamera = &cam;
    myEnv.viewMatrix = cam.matrix();
    myEnv.clipMode = TerrainClipMode::BELOW_WATER;
    /*const auto shortFrustum =  Frustum{ cam, m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distNearPlane() + 150.0f, m_pGraphicsSystem->fov(), m_pGraphicsSystem->aspectRatio() };
    const auto shortMatrix = glm::perspective(m_pGraphicsSystem->fov(), m_pGraphicsSystem->aspectRatio(), m_pGraphicsSystem->distNearPlane(), m_pGraphicsSystem->distNearPlane() + 150.0f);
    myEnv.frustum = shortFrustum;
    myEnv.projectionMatrix = shortMatrix;*/

    m_fbo.bindReflectionFramebuffer();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_pModelRenderer->render(myEnv);
    m_pTerrainRenderer->render(myEnv);

    myEnv.pCamera = env.pCamera;
    myEnv.viewMatrix = env.pCamera->matrix();
    myEnv.clipMode = TerrainClipMode::ABOVE_WATER;
    myEnv.fWaterLevel = m_fWaterLevel + 1.0f;
    m_fbo.bindRefractionFramebuffer();
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    m_pModelRenderer->render(myEnv);
    m_pTerrainRenderer->render(myEnv);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, nLastFBO);
    glViewport(0, 0, m_pGraphicsSystem->displayWidth(), m_pGraphicsSystem->displayHeight());

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(m_pQuad->vao());
    m_pPreviewShader->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo.refractionDepthTexture());
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    m_pPreviewShader->unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    prepareRendering(env);

    glCall(glActiveTexture(GL_TEXTURE0));
    glCall(glBindTexture(GL_TEXTURE_2D, m_fbo.reflectionTexture()));
    glCall(glActiveTexture(GL_TEXTURE1));
    glCall(glBindTexture(GL_TEXTURE_2D, m_fbo.refractionTexture()));
    glCall(glActiveTexture(GL_TEXTURE2));
    glCall(glBindTexture(GL_TEXTURE_2D, m_fbo.refractionDepthTexture()));

     for (auto pTile : vTiles) {
        auto& renderObj = std::invoke([](WaterRendererGL* pRend, WaterTile* pTile) -> decltype(auto) {
            auto iter = pRend->m_mTileObjects.find(pTile);
            if (iter != pRend->m_mTileObjects.end())
                return (iter->second);
            return (pRend->registerWaterTile(pTile));
        }, this, pTile);
        glBindVertexArray(renderObj.vao());

        auto modelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3{ pTile->position().x, 0.0, pTile->position().y });
        //modelMatrix = glm::scale(modelMatrix, glm::vec3{ pTile->size(), 1.0f, pTile->size() });
        m_pShaderProgram->loadMatrix4f(m_nModelMatrixID, modelMatrix);
        /*
        m_pShaderConfig->loadModelMatrix(modelMatrix);
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vQuadVertices.size()));
        */
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderObj.indexBufferAt(0));
        glDrawElements(GL_TRIANGLES, renderObj.numIndices(), GL_UNSIGNED_INT, (void*) 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    finishRendering();
}

void WaterRendererGL::finishRendering() noexcept {
    glCall(glDisable(GL_BLEND));
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

#define M_PI 3.1415926535897932384626433832795

layout(location = 0) in vec3 vertexPos;

out vec4 clipSpace;
out float fresnelFactor;

uniform vec3 cameraPos;
uniform float waterLevel;
uniform float time;
uniform float gridSize;
uniform mat4 modelMatrix;
uniform mat4 perspectiveViewMatrix;


vec3 applyDisplacement(vec3 vertex) {
    vec3 tmp = vertex;
    float x = vertex.x / gridSize;
    float z = vertex.z / gridSize;
    float multiplier = gridSize / 10.0f;
    vertex.z = vertex.z + multiplier * sin(x * M_PI / 4.0 - time);
    vertex.x = vertex.x + multiplier * cos(z * M_PI / 4.0 - time);
    vertex.y = vertex.y + multiplier * sin(x * M_PI / 4.0 - time);
    return vertex;
}

void main() {
    vec3 vertex = vec3(vertexPos.x, waterLevel, vertexPos.z);
    vec4 worldPos = modelMatrix * vec4(applyDisplacement(vertex), 1);
    fresnelFactor = dot(vec3(0, 1, 0), normalize(cameraPos - worldPos.xyz));
    clipSpace = perspectiveViewMatrix * worldPos;
    gl_Position = clipSpace;
}

)";
}

const char* WaterRendererGL::getFragmentShaderSource() noexcept {
    return R"(
#version 410

in vec4 clipSpace;
in float fresnelFactor;
out vec4 outColor;

uniform vec2 distPlanes; // near and far
uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D refractionDepthTexture;

float toLinearDepth(float depth) 
{
    float distNearPlane = distPlanes.x;
    float distFarPlane = distPlanes.y;
    float z = depth * 2.0 - 1.0; // back to NDC 
    float frustumDepth = distFarPlane - distNearPlane;
    float result = (2.0 * distNearPlane * distFarPlane) / (distFarPlane + distNearPlane - z * frustumDepth);
    return result;
}

float waterDepth(vec2 texCoord) {
    float terrainDist = toLinearDepth(texture(refractionDepthTexture, texCoord).r);
    float waterDist = toLinearDepth(gl_FragCoord.z);    // current depth (== distance to water)
    return terrainDist - waterDist;
}

void main() {
    const vec4 waterColor = vec4(0.607, 0.867, 0.851, 1.0);

    vec2 ndc = (clipSpace.xy / clipSpace.w) * 0.5 + 0.5;
    vec4 reflectionColor = texture(reflectionTexture, vec2(ndc.x, 1.0 - ndc.y));
    vec4 refractionColor = texture(refractionTexture, ndc);

    float waterDepth = waterDepth(ndc);
    float murkiness = smoothstep(0, 15, waterDepth);
    refractionColor = mix(refractionColor, waterColor, murkiness);

    outColor = mix(reflectionColor, refractionColor, fresnelFactor);
    outColor.a = clamp(waterDepth, 0.0, 1.0);
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
    const auto vNormals = g.generateNormals(vMesh, vIndices);

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
    insert_iter->second.addIndexBuffer(buffers[1], vIndices.size());
    return insert_iter->second;
}

}

#endif
