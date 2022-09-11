//
//  PathSegmentRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 30.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/RenderEnvironment.h>
#include <Components/PathSegmentComponent.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <Entities/EntityManager.h>
#include <iostream>
#include <ShadersGL/ShaderProgramGL.h>
#include <ShadersGL/PathSegmentShaderConfigurator.h>
#include <Components/TransformComponent.h>
#include <Components/SaveComponent.h>
#include <Components/SharedComponent.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Renderer/OpenGL/PathSegmentRendererGL.h>



namespace Ice {

PathSegmentRendererGL::PathSegmentRendererGL() {
    m_pShaderProgram = RenderToolsGL::createShaderProgram("PathSegment", std::make_unique<PathSegmentShaderConfigurator>());
    m_pShaderConfig = static_cast<Shader3dConfigurator*>(m_pShaderProgram->configurator());
}

void PathSegmentRendererGL::render(const RenderEnvironment& env, const std::vector<Entity>& vEnts) noexcept {
    for (auto e : vEnts) {
        if (!entityManager.hasComponent<SaveComponent>(e))
            continue;
        if (m_mRenderObjects.find(e) == m_mRenderObjects.end()) {
            m_mRenderObjects.emplace(e, std::make_unique<RenderObjectGL>(RenderToolsGL::createVAO()));
            auto& pObj = m_mRenderObjects.at(e);
            
            const auto& pathSeg = entityManager.getComponent<PathSegmentComponent>(e);
            
            std::vector<GLfloat> vVerts;
            for (const auto& [dir, verts] : pathSeg.m_pathSegment.vertices()) {
                for (const auto& v : verts) {
                    vVerts.emplace_back(v[0]);
                    vVerts.emplace_back(v[1]);
                    vVerts.emplace_back(v[2]);
                }
            }
            pObj->addBuffer(RenderToolsGL::loadFloatBuffer(GL_ARRAY_BUFFER, static_cast<GLfloat*>(&vVerts[0]), vVerts.size(), GL_STATIC_DRAW));

            glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
            glCall(glEnableVertexAttribArray(0));
            
            m_mNumLines[e] = vVerts.size() - 1;
        }
        
        const auto& pObj = m_mRenderObjects.at(e);
        glCall(glBindVertexArray(pObj->vao()));

        const auto& transf = std::invoke([e]() -> decltype(auto) {
            if (entityManager.hasComponent<TransformComponent>(e)) {
                return entityManager.getComponent<TransformComponent>(e);
            }
            auto& sharedComp = entityManager.getComponent<SharedComponent<TransformComponent>>(e);
            return *sharedComp.m_pComponent;
        });

        m_pShaderProgram->use();
        m_pShaderConfig->loadUniforms(env);
        m_pShaderConfig->loadModelMatrix(glm::translate(glm::mat4{ 1.0f }, glm::vec3{ 0.0f, .1f, 0.0f }) * glm::mat4{ 1.0f });
        //glCall(glBindBuffer(GL_ARRAY_BUFFER, pObj->bufferAt(0)));
        glCall(glDrawArrays(GL_LINES, 0, m_mNumLines[e]));
        //glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        glCall(glBindVertexArray(0));
        m_pShaderProgram->unuse();
    }
    
}

}
