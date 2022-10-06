//
//  WidgetRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glad.h>
#include <Renderer/OpenGL/WidgetRendererGL.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <ShadersGL/WidgetShaderConfigurator.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <GUI/WidgetManager.h>
#include <System/SystemServices.h>
#include <GUI/TextLabel.h>
#include <GUI/Button.h>
#include <GUI/ImageView.h>
#include <GUI/ModelPreview.h>
#include <Interfaces/IModelRenderer.h>
#include <Renderer/RenderEnvironment.h>
#include <System/ModelInstance.h>
#include <System/Model.h>
#include <Components/TransformComponent.h>
#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <GUI/ScrollWidget.h>
#include <GUI/DropDown.h>
#include <GUI/StackPanel.h>

namespace Ice {

WidgetRendererGL::WidgetRendererGL() {
    
    assert(m_bInstanced == false);

    m_pQuad = std::make_unique<RenderObjectGL>(RenderToolsGL::loadVerticesToVAO(m_vQuadVertices));
    glCall(glEnableVertexAttribArray(0));
    glCall(glBindVertexArray(0));

    m_pShaderProgram = RenderToolsGL::createShaderProgram("Widget", std::make_unique<WidgetShaderConfigurator>());
    m_pShaderConfig = dynamic_cast<WidgetShaderConfigurator*>(m_pShaderProgram->configurator());

    m_pTextLabelRenderer = systemServices.getTextRenderer();
    m_pModelRenderer = systemServices.getModelRenderer();
    
    m_bInstanced = true;
}

void WidgetRendererGL::_renderWidget(const GUI::Widget *pWidget, const std::function<void()>& additionalSetup, const std::function<void()>& additionalDeinit) {
    glCall(glDisable(GL_DEPTH_TEST));
    glCall(glEnable(GL_BLEND));
    glCall(glBindVertexArray(m_pQuad->vao()));
    glCall(m_pShaderProgram->use());

    m_pShaderConfig->loadScaleFactors(glm::vec2{ pWidget->width(), pWidget->height() });
    m_pShaderConfig->loadPosition(pWidget->position());
    float fxOffset;
    float fyOffset;
    using namespace GUI;
    switch (pWidget->horizAlignment()) {
        case HorizontalAlignment::LEFT:
            fxOffset = 0.0f;
            break;
        case HorizontalAlignment::CENTER:
            fxOffset = -0.5f;
            break;
        case HorizontalAlignment::RIGHT:
            fxOffset = -1.0f;
            break;
        default:
            break;
    }
    switch (pWidget->vertAlignment()) {
        case VerticalAlignment::BOTTOM:
            fyOffset = 0.0f;
            break;
        case VerticalAlignment::MIDDLE:
            fyOffset = -0.5f;
            break;
        case VerticalAlignment::TOP:
            fyOffset = -1.0f;
            break;
        default:
            break;
    }
    m_pShaderConfig->loadAlignOffset(fxOffset, fyOffset);
    m_pShaderConfig->loadColor(pWidget->color());
    m_pShaderConfig->loadForegroundColor(pWidget->foregroundColor());
    m_pShaderConfig->loadBorderThickness(systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(pWidget->borderThickness()));
    //if (pWidget->borderThickness() > 0.0f) {
        m_pShaderConfig->loadBorderColor(pWidget->borderColor());
    //}
    m_pShaderConfig->loadCornerRadius(systemServices.getGraphicsSystem()->getNormalizedPixelsHorizontal(pWidget->cornerRadius()));
    
    auto [l, b, r, t] = pWidget->clipFrame();
    glm::vec2 minPoint{ l, b };
    glm::vec2 maxPoint{ r, t };
    m_pShaderConfig->loadMinPoint(minPoint);
    m_pShaderConfig->loadMaxPoint(maxPoint);
    
    if (additionalSetup)
        additionalSetup();
    
    glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
    
    if (additionalDeinit)
        additionalDeinit();
    
    m_pShaderProgram->unuse();
    glCall(glBindVertexArray(0));

    for (const auto& info : pWidget->children()) {
        info.child()->render(this);
    }
 
    glCall(glDisable(GL_BLEND));
    glCall(glEnable(GL_DEPTH_TEST));
}

void WidgetRendererGL::renderWidget(const GUI::Widget *pWidget) {
    if (pWidget->visible())
        _renderWidget(pWidget);
}

void WidgetRendererGL::renderWidget(const GUI::Button* pButton) {
    if (pButton->visible())
        _renderWidget(pButton);
}

void WidgetRendererGL::renderWidget(const GUI::ScrollWidget* pScroll) {
    if (pScroll->visible())
        _renderWidget(pScroll);
}

void WidgetRendererGL::renderWidget(GUI::TextLabel *pLabel) {
    if (pLabel->visible()) {
        _renderWidget((GUI::Widget*)(pLabel));
        m_pTextLabelRenderer->render(pLabel);
    }
}

void WidgetRendererGL::renderWidget(const GUI::ImageView* pImageView) {
    if (!pImageView->visible())
        return;
    
    auto pTex = static_cast<TextureGL*>(pImageView->image()->data());
    _renderWidget((GUI::Widget*)pImageView, [this, pTex, pImageView]() {
        auto [minPoint, maxPoint] = pImageView->texCoord();
        m_pShaderConfig->loadIsTextured(true);
        m_pShaderConfig->loadMinMaxTexCoord(minPoint, maxPoint);
        pTex->bind(m_pShaderConfig->textureUniformId());
    }, [this, pTex]() {
        m_pShaderConfig->loadIsTextured(false);
        pTex->unbind();
    });
}

void WidgetRendererGL::renderWidget(const GUI::ModelPreview* pPreview) {
    //static int nCount{ 0 };
    //static std::chrono::duration avgDur{ std::chrono::nanoseconds{ 0 } };
    //std::chrono::time_point<std::chrono::high_resolution_clock> lastTime = std::chrono::high_resolution_clock::now();

    sModelPreviewInfo *pInfo{ nullptr };
    auto iter = m_mModelPreviewInfo.find(pPreview);
    if (iter == m_mModelPreviewInfo.end()) {
        std::uint16_t width, height;
        width = pPreview->contentWidth() * 1.0f / systemServices.getGraphicsSystem()->getHorizontalPixelScale();
        height = pPreview->contentHeight() * 1.0f / systemServices.getGraphicsSystem()->getVerticalPixelScale();
        
        sModelPreviewInfo info;
        info.fbo = FramebufferObjectGL{ width, height };
        info.fbo.createTextureAttachment();
        info.fbo.createDepthAttachment();
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            return;
        
        info.nTexId = info.fbo.textureAttachmentId();
        info.projMatrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 500.0f);
        auto [myIter, bSucc] = m_mModelPreviewInfo.emplace(pPreview, std::move(info));
        iter = myIter;
    }
    pInfo = &iter->second;
    if (pPreview->graphicsDirty()) {

        RenderEnvironment env;
        env.viewMatrix = pPreview->viewMatrix();
        env.projectionMatrix = pInfo->projMatrix;
        
        TransformComponent transf;
        transf.m_transform = pPreview->modelMatrix();
        ModelInstance inst;
        inst.pTransform = &transf;
        
        //RenderToolsGL::bindFramebuffer(pInfo->nFramebuffer, width, height);
        pInfo->fbo.bind(systemServices.getGraphicsSystem()->displayWidth(), systemServices.getGraphicsSystem()->displayHeight());
        glCall(glClearColor(pPreview->color().r, pPreview->color().g, pPreview->color().b, pPreview->color().a));
        glCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        //std::vector<std::pair<Model, std::vector<ModelInstance*>>> mData;
        std::unordered_map<Model, std::vector<ModelInstance*>> mData;
        for (const auto& model : pPreview->models())
            mData.emplace(std::piecewise_construct, std::forward_as_tuple(model), std::forward_as_tuple<std::initializer_list<ModelInstance*>>({ &inst }));
        //{ { pPreview->model(), { &inst } } };
        m_pModelRenderer->render(env, mData);

        glCall(glGenerateMipmap(GL_TEXTURE_2D));
        pInfo->fbo.unbind();
    }
    
    //auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - lastTime);
    //avgDur += dur;
    //if (++nCount > 500) {
    //    std::cout << std::dec << (avgDur / nCount).count() << "ns" << std::endl;
    //    nCount = 0;
    //    avgDur = std::chrono::nanoseconds{ 0 };
    //}

    _renderWidget((GUI::Widget*)pPreview, [this, pInfo]() {
        glm::vec2 minPoint{ 0.0f, 0.0f };
        glm::vec2 maxPoint{ 1.0f, 1.0f };
        m_pShaderConfig->loadIsTextured(true);
        m_pShaderConfig->loadMinMaxTexCoord(minPoint, maxPoint);
        glCall(glActiveTexture(GL_TEXTURE0));
        glCall(glBindTexture(GL_TEXTURE_2D, pInfo->nTexId));
    }, [this]() {
        m_pShaderConfig->loadIsTextured(false);
        glCall(glBindTexture(GL_TEXTURE_2D, 0));
    });
}

void WidgetRendererGL::render(const RenderEnvironment& env, const std::vector<GUI::Widget*> &vWidgets) noexcept {
//    static std::chrono::time_point LastMeasurementTime = std::chrono::high_resolution_clock::now();
//    static int nMeasurements{ 0 };
//    static std::chrono::duration<float, std::nano> avgDur;
//
//    std::chrono::time_point<std::chrono::high_resolution_clock> beginTime = std::chrono::high_resolution_clock::now();
    
    //glCall(glEnable(GL_CLIP_DISTANCE0));
    //glCall(glEnable(GL_CLIP_DISTANCE1));
    //glCall(glEnable(GL_CLIP_DISTANCE2));
    //glCall(glEnable(GL_CLIP_DISTANCE3));
    
    m_pShaderProgram->use();
    m_pShaderConfig->loadAspectRatio(env.fAspectRatio);
    m_pShaderProgram->unuse();
    for (auto pWidget : vWidgets) {
        if (pWidget->visible())
            pWidget->render(this);
    }
    
    //glCall(glDisable(GL_CLIP_DISTANCE0));
    //glCall(glDisable(GL_CLIP_DISTANCE1));
    //glCall(glDisable(GL_CLIP_DISTANCE2));
    //glCall(glDisable(GL_CLIP_DISTANCE3));
    
//    std::chrono::time_point<std::chrono::high_resolution_clock> endTime = std::chrono::high_resolution_clock::now();
//    auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - beginTime);
//    avgDur += dur;
//    ++nMeasurements;
//    if (nMeasurements == 500) {
//        std::cout << "Avg time (ms): " << std::chrono::duration_cast<std::chrono::milliseconds>(avgDur / nMeasurements).count() << std::endl;
//        nMeasurements = 0;
//        avgDur = std::chrono::nanoseconds{ 0 };
//    }
}

}
