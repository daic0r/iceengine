//
//  TerrainRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 21.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/OpenGL/TerrainRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <World/Terrain.h>
#include <iostream>
#include <ShadersGL/TerrainShaderConfigurator.h>
#include <System/Config.h>
#include <System/SystemServices.h>
#include <Interfaces/IShaderManager.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Interfaces/ITexture.h>
#include <ShadersGL/TerrainHighlightShaderConfigurator.h>
#include <Renderer/RenderEnvironment.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Renderer/OpenGL/ModelRendererGL.h>
#include <Interfaces/IModelRenderer.h>
#include <Renderer/OpenGL/ShadowMapRendererGL.h>
#include <Renderer/OpenGL/GraphicsSystemGL.h>
#include <System/Light.h>

namespace Ice {

TerrainRendererGL::TerrainRendererGL() {
//    auto pConfig = std::make_unique<TerrainShaderConfigurator>();
//
//    std::string vertexShaderPath{ Config::BasePath };
//    vertexShaderPath.append("ShadersGL/TerrainVertexShader.glsl");
//    std::string fragmentShaderPath{ Config::BasePath };
//    fragmentShaderPath.append("ShadersGL/TerrainFragmentShader.glsl");
//
//    shaderManager->registerShaderProgram("terrainShader", vertexShaderPath, fragmentShaderPath, std::move(pConfig));
//    m_pShaderProgram = shaderManager->getShader("terrainShader");
//    m_pShaderConfig = dynamic_cast<TerrainShaderConfigurator*>(m_pShaderProgram->configurator());

#ifdef _DEBUG_OCTREE
    m_pOctreeDebugger = std::make_unique<OctreeDebuggerGL>();
#endif
    
    m_pShaderProgram = RenderToolsGL::createShaderProgram("Terrain", std::make_unique<TerrainShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<TerrainShaderConfigurator*>(m_pShaderProgram->configurator());
    
    m_pTerrainHighlightShaderProgram = RenderToolsGL::createShaderProgram("TerrainHighlight", std::make_unique<TerrainHighlightShaderConfigurator>());
    m_pTerrainHighlightShaderConfig = dynamic_cast<TerrainHighlightShaderConfigurator*>(m_pTerrainHighlightShaderProgram->configurator());
    
    glCall(glGenVertexArrays(1, &m_quadVAO));
    glCall(glBindVertexArray(m_quadVAO));
    glCall(glGenBuffers(1, &m_quadVBO));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO));
    glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 18, nullptr, GL_DYNAMIC_DRAW));
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
    glCall(glEnableVertexAttribArray(0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindVertexArray(0));

	m_pModelRenderer = dynamic_cast<ModelRendererGL*>(systemServices.getModelRenderer());
    m_pGraphicsSystem = dynamic_cast<GraphicsSystemGL*>(systemServices.getGraphicsSystem());
	m_pShaderProgram->use();
    m_pShaderConfig->loadWaterLevelAndClipPlaneY(0.0f, 0);
	//m_pShaderConfig->loadShadowDistance(100.0f);
	//m_pShaderConfig->loadShadowMargin(10.0f);
	m_pShaderProgram->unuse();
}

TerrainRendererGL::~TerrainRendererGL() {
    glCall(glDeleteBuffers(1, &m_quadVBO));
    glCall(glDeleteVertexArrays(1, &m_quadVAO));
}

void TerrainRendererGL::prepareRendering(const RenderEnvironment& env, const std::vector<Terrain>& vTerrains) noexcept {
    for (auto& terrain : vTerrains) {
        if (auto iter = m_mTerrains.find(terrain.pTerrain->m_terrain.terrainId()); iter != m_mTerrains.end()) {
            dynamic_cast<DynamicVertexAttributeGL<glm::vec4>&>(iter->second.attribute(1)).commitUpdates();
            continue;
        }
        
        VAO vao;
        vao.create();
        vao.bind();
        //auto pGlObject = std::make_unique<RenderObjectGL>(RenderToolsGL::createVAO());
        glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);
        //GLuint nVertexBuffer = RenderToolsGL::loadFloatBuffer(vao, GL_ARRAY_BUFFER, terrain.pMesh->vertices(), GL_STATIC_DRAW);
        //glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
        vao.addStaticVertexAttribute<glm::vec3>(0);
        vao.attribute(0).setData(&terrain.pMesh->vertices());
        vao.attribute(0).connect();
        
        vao.addDynamicVertexAttribute<glm::vec4>(1);
        vao.attribute(1).setData(&terrain.pMesh->colors());
        vao.attribute(1).connect();

        //m_pVertexColors = std::make_unique<DynamicVertexAttributeGL<glm::vec4>>(1);
        //m_pVertexColors->setData(std::move(vColorBuf));
        //m_pVertexColors->connect();
        
        vao.addStaticVertexAttribute<glm::vec3>(2);
        vao.attribute(2).setData(&terrain.pMesh->normals());
        vao.attribute(2).connect();
 
        glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        //GLuint nColorBuffer = RenderToolsGL::loadFloatBuffer(pGlObject->vao(), GL_ARRAY_BUFFER, terrain.pMesh->colors(), GL_STATIC_DRAW);
        //glCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, nullptr));
        //GLuint nNormalBuffer = RenderToolsGL::loadFloatBuffer(pGlObject->vao(), GL_ARRAY_BUFFER, terrain.pMesh->normals(), GL_STATIC_DRAW);
        //glCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
        /*
        glCall(glEnableVertexAttribArray(0));
        glCall(glEnableVertexAttribArray(1));
        glCall(glEnableVertexAttribArray(2));
        GLuint nElementBuffer;
        glCall(glGenBuffers(1, &nElementBuffer));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nElementBuffer));
        glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * terrain.pMesh->indices().size(), &(terrain.pMesh->indices()[0]), GL_STATIC_DRAW));
        */ 
        vao.setIndexBuffer(terrain.pMesh->indices());
        /*
        pGlObject->addBuffer(nVertexBuffer);
        //pGlObject->addBuffer(nColorBuffer);
        pGlObject->addBuffer(nNormalBuffer);
        pGlObject->addIndexBuffer(nElementBuffer);
        */
        
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        glCall(glBindVertexArray(0));
        
        //m_mTerrains.emplace(terrain.pTerrain->id(), std::move(pGlObject));
        auto [iter, _] = m_mTerrains.emplace(terrain.pTerrain->id(), std::move(vao));

        ///////////////////////////////////////////////////////////////////
        // Link up pointer
        terrain.pTerrain->m_pColorAttrib = dynamic_cast<DynamicVertexAttributeGL<glm::vec4>*>(&iter->second.attribute(1));
        ///////////////////////////////////////////////////////////////////

        /*
        attrib.update(1, glm::vec4{ 1.0f, 0.0f, 0.0f, 1.0f });
        attrib.update(2, glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f });
        attrib.commitUpdates();
        */
    }

    //glCall(glEnable(GL_PRIMITIVE_RESTART));
    glCall(glEnable(GL_DEPTH_TEST));
    glCall(glEnable(GL_CULL_FACE));
    //glCall(glPrimitiveRestartIndex(0xFFFF));
    m_pShaderProgram->use();
    m_pShaderConfig->loadUniforms(env);
	//m_pShaderConfig->loadShadowProjViewMatrix(m_pModelRenderer->shadowProjectionViewMatrix());
    if (env.pSun) {
        m_pGraphicsSystem->commonUBO().bind();
        m_pGraphicsSystem->commonUBO().loadSunPosition(env.pSun->position());
        m_pGraphicsSystem->commonUBO().loadSunColor(env.pSun->color());
        m_pGraphicsSystem->commonUBO().loadSunAmbient(env.pSun->ambient());
        m_pGraphicsSystem->commonUBO().unbind();
    }
    if (env.fWaterLevel.has_value()) {
        m_pShaderConfig->loadWaterLevelAndClipPlaneY(*env.fWaterLevel, static_cast<int>(env.clipMode));
        glEnable(GL_CLIP_DISTANCE0);
    }        

	glCall(glActiveTexture(GL_TEXTURE1));
	glCall(glBindTexture(GL_TEXTURE_2D, dynamic_cast<ShadowMapRendererGL*>(systemServices.getShadowMapRenderer())->getShadowDepthTextureId()));
	//glCall(glBindTexture(GL_TEXTURE_2D, m_pModelRenderer->getShadowDepthTextureId()));
}

void TerrainRendererGL::render(const RenderEnvironment &env, const std::vector<Terrain>& vTerrains) noexcept {
    prepareRendering(env, vTerrains);
    for (const auto& terrain : vTerrains) {
        auto iter = m_mTerrains.find(terrain.pTerrain->m_terrain.terrainId());

        if (iter == m_mTerrains.end())
            continue;
        
        const auto& vao = iter->second;
        
        glm::mat4 modelMatrix = glm::translate(glm::mat4{1.0f}, glm::vec3{ terrain.pTerrain->m_terrain.gridX(), 0.0f, terrain.pTerrain->m_terrain.gridZ() });
        m_pShaderConfig->loadModelMatrix(modelMatrix);

        vao.bind();
        vao.bindIndexVBO();
        
        //auto pTex = terrain.pTexture->texture();
        //if (pTex)
        //    pTex->bind(m_pShaderConfig->textureUniformId());

        glCall(glDrawElements(GL_TRIANGLES, vao.indexBuffer().size(), GL_UNSIGNED_INT, nullptr));

        //if (pTex)
        //    terrain.pTexture->texture()->unbind();
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        vao.unbind();
        //glCall(glBindVertexArray(0));


        /*
        glCall(glBindVertexArray(m_quadVAO));
        m_pTerrainHighlightShaderProgram->use();
        m_pTerrainHighlightShaderConfig->loadUniforms(env);
        
        float fSize = 1.0f;
        auto tilePair = terrain.pTerrain->getSelectedTile(*env.pCamera, 1.0f, fSize);

        auto markerPos = glm::vec3{terrain.pTerrain->worldX() + (static_cast<float>(tilePair.first) * terrain.pTerrain->tileSize()), 0.0f, terrain.pTerrain->worldZ() + (static_cast<float>(tilePair.second) * terrain.pTerrain->tileSize())};
        modelMatrix = glm::translate(glm::mat4{1.0f}, markerPos);
        m_pTerrainHighlightShaderConfig->loadModelMatrix(modelMatrix);

        std::array<GLfloat, 12> arVertices;
        for (int i = 0; i < 4; ++i) {
            float fHeight;
            float fX = markerPos.x + HighlightQuad[i * 2] * terrain.pTerrain->tileSize() * fSize;
            float fZ = markerPos.z + HighlightQuad[i * 2 + 1] * terrain.pTerrain->tileSize() * fSize;
            terrain.pTerrain->getHeight(fX, fZ, fHeight);
            arVertices[i * 3] = HighlightQuad[i * 2] * terrain.pTerrain->tileSize() * fSize;
            arVertices[i * 3 + 1] = fHeight + 1.0f;
            arVertices[i * 3 + 2] = HighlightQuad[i * 2 + 1] * terrain.pTerrain->tileSize() * fSize;
        }
        glCall(glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO));
        glCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * arVertices.size(), &arVertices[0]));
        glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
        glCall(glBindVertexArray(0));
        m_pTerrainHighlightShaderProgram->unuse();
        */
    }
    
    finishRendering(env);
}

void TerrainRendererGL::finishRendering(const RenderEnvironment& env) noexcept {
    if (env.fWaterLevel.has_value()) {
        m_pShaderConfig->loadWaterLevelAndClipPlaneY(0.0f, 0);
        glDisable(GL_CLIP_DISTANCE0);
    }        
    m_pShaderProgram->unuse();
    //glCall(glDisable(GL_PRIMITIVE_RESTART));
    glCall(glDisable(GL_DEPTH_TEST));
    glCall(glDisable(GL_CULL_FACE));

#ifdef _DEBUG_OCTREE
    if (m_pOctreeDebugger)
        m_pOctreeDebugger->render(env, m_pOctree);
#endif
}

}
