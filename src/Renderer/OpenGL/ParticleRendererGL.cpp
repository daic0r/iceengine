//
//  ParticleRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifdef RENDERER_OPEN_GL

#include <glad/glad.h>
#include <Renderer/OpenGL/ParticleRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <ShadersGL/ParticleShaderConfigurator.h>
#include <System/Scene.h>
#include <System/Camera.h>
#include <Particles/Particle.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat3x3.hpp>
#include <Renderer/RenderEnvironment.h>
#include <System/SystemServices.h>
#include <Interfaces/IShaderManager.h>
#include <Interfaces/IShaderProgram.h>
#include <System/Config.h>
#include <iostream>
#include <System/Tools.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <System/Camera.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <algorithm>
#include <Particles/ParticleSystem.h>

namespace Ice {

const std::vector<glm::vec2> ParticleRendererGL::m_vQuadVertices {
    glm::vec2{ -0.5f, 0.5f },
    glm::vec2{ -0.5f, -0.5f },
    glm::vec2{ 0.5f, 0.5f },
    glm::vec2{ 0.5f, -0.5f }
};

bool ParticleRendererGL::m_bInstanced{ false };

ParticleRendererGL::ParticleRendererGL() {
    assert(!m_bInstanced);
    
    std::unique_ptr<IShaderConfigurator> pConfig = std::make_unique<ParticleShaderConfigurator>();
    
//    std::string vertexShaderPath{ Config::BasePath };
//    vertexShaderPath.append("ShadersGL/ParticleVertexShader.glsl");
//    std::string fragmentShaderPath{ Config::BasePath };
//    fragmentShaderPath.append("ShadersGL/ParticleFragmentShader.glsl");
//
//    shaderManager->registerShaderProgram("particleShader", vertexShaderPath, fragmentShaderPath, std::move(pConfig));
//    m_pShaderProgram = shaderManager->getShader("particleShader");
//    m_pShaderConfig = dynamic_cast<ParticleShaderConfigurator*>(m_pShaderProgram->configurator());
    m_pShaderProgram = RenderToolsGL::createShaderProgram("Particle", std::make_unique<ParticleShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<ParticleShaderConfigurator*>(m_pShaderProgram->configurator());

    m_pQuad = std::make_unique<RenderObjectGL>(RenderToolsGL::loadVerticesToVAO(m_vQuadVertices));
    m_VBO = m_pQuad->createEmptyVBO(10 * MAX_PARTICLES);
    m_pQuad->addInstanceVertexAttribute(m_VBO, 1, 3, 10, 0);
    m_pQuad->addInstanceVertexAttribute(m_VBO, 2, 1, 10, 3);
    m_pQuad->addInstanceVertexAttribute(m_VBO, 3, 1, 10, 4);
    m_pQuad->addInstanceVertexAttribute(m_VBO, 4, 4, 10, 5);
    m_pQuad->addInstanceVertexAttribute(m_VBO, 5, 1, 10, 9);
	glCall(glEnableVertexAttribArray(0));
    glCall(glEnableVertexAttribArray(1));
    glCall(glEnableVertexAttribArray(2));
    glCall(glEnableVertexAttribArray(3));
    glCall(glEnableVertexAttribArray(4));
    glCall(glEnableVertexAttribArray(5));
    glCall(glBindVertexArray(0));

    m_bInstanced = true;
}

ParticleRendererGL::ParticleRendererGL(const glm::mat4& proj) : ParticleRendererGL{} {
    setProjectionMatrix(proj);
}

void ParticleRendererGL::prepareRendering(const RenderEnvironment& env) noexcept {
    //glCall(glDisable(GL_DEPTH_TEST)); // <---- IMPORTANT, otherwise blank screen (if Renderer is not called)!
    glCall(m_pShaderProgram->use());
    glCall(m_pShaderConfig->loadProjectionMatrix(env.projectionMatrix));
    glCall(glBindVertexArray(m_pQuad->vao()));
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    glCall(glDepthMask(false));
}

void ParticleRendererGL::render(const RenderEnvironment& env, std::vector<ParticleSystem*>& vParticleSystems) noexcept {
	prepareRendering(env);

    auto viewMatrix = env.viewMatrix;
    auto pCamera = env.pCamera;

    m_pShaderConfig->loadViewMatrix(viewMatrix);
    
    for (const auto& pSystem : vParticleSystems) {
		if (pSystem->particles().empty())
			continue;

        // Sort particles according to camera distance
        //std::vector<std::shared_ptr<Particle>> vSorted = kvp.second;
        std::sort(pSystem->particles().begin(), pSystem->particles().end(), [pCamera](const auto& a, const auto& b) {
            auto distanceA = glm::length(a.position() - pCamera->position());
            auto distanceB = glm::length(b.position() - pCamera->position());
            return distanceB < distanceA;
        });
        
        // Bind texture for all subsequently rendered particles
		auto pTex = dynamic_cast<TextureGL*>(pSystem->texture());
        pTex->bind(m_pShaderConfig->textureUniformId());
        m_pShaderConfig->loadCellDims(pTex->cellWidth(), pTex->cellHeight());

        m_vRenderBuffer.clear();
        for (const auto& particle : pSystem->particles()) {
            m_vRenderBuffer.emplace_back(particle.position().x);
            m_vRenderBuffer.emplace_back(particle.position().y);
            m_vRenderBuffer.emplace_back(particle.position().z);
            m_vRenderBuffer.emplace_back(particle.rotation());
            m_vRenderBuffer.emplace_back(particle.scale());
            m_vRenderBuffer.emplace_back(particle.texOffset1().first);
            m_vRenderBuffer.emplace_back(particle.texOffset1().second);
            m_vRenderBuffer.emplace_back(particle.texOffset2().first);
            m_vRenderBuffer.emplace_back(particle.texOffset2().second);
            m_vRenderBuffer.emplace_back(particle.blendFactor());
        }
        RenderToolsGL::loadVBOData(m_VBO, m_vRenderBuffer);
        
        // RENDER!
        glCall(glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, pSystem->particles().size()));
        
        pTex->unbind();
    }
	finishRendering();
}

void ParticleRendererGL::finishRendering() noexcept {
    glCall(glBindVertexArray(0));
    //glCall(glEnable(GL_DEPTH_TEST));
    glCall(m_pShaderProgram->unuse());
    glCall(glDisable(GL_BLEND));
    glCall(glDepthMask(true));
}

void ParticleRendererGL::setProjectionMatrix(const glm::mat4 &proj) noexcept {
    m_projection = proj;
    m_pShaderProgram->use();
    m_pShaderConfig->loadProjectionMatrix(proj);
    m_pShaderProgram->unuse();
}

}

#endif
