//
//  TextRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 17.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/OpenGL/TextRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>
#include <ShadersGL/ShaderProgramGL.h>
#include <System/SystemServices.h>
#include <Interfaces/IShaderManager.h>
#include <ShadersGL/TextShaderConfigurator.h>
#include <System/Config.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <GUI/TextLabel.h>
#include <glm/gtx/string_cast.hpp>

namespace Ice {

TextRendererGL::TextRendererGL() {
//    auto pConfig = std::make_unique<TextShaderConfigurator>();
//
//    std::string vertexShaderPath{ Config::BasePath };
//    vertexShaderPath.append("ShadersGL/TextVertexShader.glsl");
//    std::string fragmentShaderPath{ Config::BasePath };
//    fragmentShaderPath.append("ShadersGL/TextFragmentShader.glsl");
//
//    shaderManager->registerShaderProgram("textShader", vertexShaderPath, fragmentShaderPath, std::move(pConfig));
//    m_pShaderProgram = shaderManager->getShader("textShader");
//    m_pShaderConfig = dynamic_cast<TextShaderConfigurator*>(m_pShaderProgram->configurator());
    
    m_pShaderProgram = RenderToolsGL::createShaderProgram("Text", std::make_unique<TextShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<TextShaderConfigurator*>(m_pShaderProgram->configurator());
}

BaseTexture* TextRendererGL::registerFontTexture(const std::string& strName, const std::string &strTextureFile) {
    if (auto iter = m_mTextures.find(strName); iter != m_mTextures.end())
        return iter->second.get();
    
    TextureGL *pTex{ nullptr };
    auto [iter, bSuccess] = m_mTextures.emplace(strName, std::make_unique<TextureGL>());
    pTex = iter->second.get();
    pTex->loadFromFile(strTextureFile);
    return pTex;
}

void TextRendererGL::finishRendering(TextureGL* pTex) noexcept {
    //glCall(glDisable(GL_MULTISAMPLE));
    m_pShaderProgram->unuse();
    pTex->unbind();
    glCall(glBindVertexArray(0));
    
    glCall(glDisable(GL_BLEND));
    glCall(glEnable(GL_DEPTH_TEST));
}

/////////////////////////////////////

void TextRendererGL::render(GUI::TextLabel* text) noexcept {
    if (text->vertexBuffer().size() == 0)
        return;

    auto pTex = prepareRendering(text);
    
    m_pShaderConfig->loadTextColor(text->style().color());
    m_pShaderConfig->loadBorderColor(text->style().borderColor());
    m_pShaderConfig->loadThickness(text->style().thickness());
    m_pShaderConfig->loadEdgeWidth(text->style().edgeWidth());
    m_pShaderConfig->loadBorderWidth(text->style().borderWidth());
    m_pShaderConfig->loadBorderEdge(text->style().borderEdge());
    m_pShaderConfig->loadShadowOffset(text->style().shadowOffset());
    //m_pShaderConfig->loadRotation(text->rotation());
    m_pShaderConfig->loadPosition(text->position());
    
//    glm::vec2 minPoint = glm::vec2 { text->left(false), text->bottom(false) };
//    glm::vec2 maxPoint = glm::vec2 { text->right(false), text->top(false) };
    auto [l, b, r, t] = text->clipFrame();
    glm::vec2 minPoint{ l, b };
    glm::vec2 maxPoint{ r, t };
    m_pShaderConfig->loadMinPoint(minPoint);
    m_pShaderConfig->loadMaxPoint(maxPoint);
    
    glDrawArrays(GL_TRIANGLES, 0, text->vertexBuffer().size() / 4);

    finishRendering(pTex);
}

TextureGL* TextRendererGL::prepareRendering(GUI::TextLabel* text) noexcept {
    m_pShaderProgram->use();
    
    TextureGL* pTex{ nullptr };
    auto labelIter = m_mLabelBuffer.find(text);
    if (labelIter != m_mLabelBuffer.end()){
        if (labelIter->first->graphicsDirty()) {
            m_mLabelBuffer.erase(labelIter);
            labelIter = m_mLabelBuffer.end();
        }
    }
    if (labelIter == m_mLabelBuffer.end()) {
        GLuint nVAO = RenderToolsGL::createVAO();
        auto obj = std::make_unique<RenderObjectGL>( nVAO );
        //GLuint nVBO = RenderToolsGL::createEmptyVBO(nVAO, 4, GL_STATIC_DRAW);
        
        GLuint nVBO;
        glCall(glGenBuffers(1, &nVBO));
        glCall(glBindBuffer(GL_ARRAY_BUFFER, nVBO));
        glCall(glBufferData(GL_ARRAY_BUFFER, text->vertexBuffer().size() * sizeof(GLfloat), &text->vertexBuffer()[0], GL_STATIC_DRAW));
        obj->addBuffer(nVBO);
        
        glCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr));
        glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), reinterpret_cast<void*>(2 * sizeof(GLfloat))));
        
        glCall(glEnableVertexAttribArray(0));
        glCall(glEnableVertexAttribArray(1));
        
        glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        
        m_mLabelBuffer.emplace(text, std::move(obj));
    } else {
        glCall(glBindVertexArray(labelIter->second->vao()));
    }

    auto texIter = m_mTextures.find(text->style().fontName());
    if (texIter == m_mTextures.end()) {
        std::string strFileName = systemServices.getConfig()->prependAssetPath("Assets/Fonts/" + text->style().fontName() + ".png");
        //std::string strFileName{ Config::BasePath };
        //strFileName += "Assets/Fonts/" + text->style().fontName() + ".png";

        pTex = static_cast<TextureGL*>(registerFontTexture(text->style().fontName(), strFileName));
    } else {
        pTex = texIter->second.get();
    }
    
    pTex->bind(m_pShaderConfig->textureUniformId());
    
    //glCall(glEnable(GL_MULTISAMPLE));
    glCall(glDisable(GL_DEPTH_TEST));
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // IMPORTANT: I don't enable clip distances here because this is assumed to be called
    // from within WidgetRendererGL, which already has enabled these
    
    return pTex;
}

}
