//
//  AnimatedModelRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 18.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/OpenGL/AnimatedModelRendererGL.h>
#include <iostream>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <System/AnimatedModel.h>
#include <System/AnimatedModelInstance.h>
#include <Interfaces/IShaderProgram.h>
#include <Interfaces/IShaderConfigurator.h>
#include <ShadersGL/AnimatedModelShaderConfigurator.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Ice {

AnimatedModelRendererGL::AnimatedModelRendererGL() noexcept {

    //setLoadAdditionalVBOFunc([](const Model* pModel, RenderObjectGL* pRenderObject) {
    //    auto pAniModel = static_cast<const AnimatedModel*>(pModel);
    //    
    //    auto pAnimated = pAniModel->pAnimatedMesh;
    //    
    //    GLuint nJointIdBuffer{ 0 };
    //    if (pAnimated != nullptr && pAnimated->m_vJointIds.size() > 0) {
    //        glCall(glGenBuffers(1, &nJointIdBuffer));
    //        glCall(glBindBuffer(GL_ARRAY_BUFFER, nJointIdBuffer));
    //        glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::ivec4) * pAnimated->m_vJointIds.size(), &pAnimated->m_vJointIds[0], GL_STATIC_DRAW));
    //        glCall(glVertexAttribIPointer(7, 4, GL_INT, sizeof(GLint) * 4, nullptr));
    //        glCall(glEnableVertexAttribArray(7));
    //    }
    //    GLuint nJointWeightsBuffer{ 0 };
    //    if (pAnimated != nullptr && pAnimated->m_vWeights.size() > 0) {
    //        glCall(glGenBuffers(1, &nJointWeightsBuffer));
    //        glCall(glBindBuffer(GL_ARRAY_BUFFER, nJointWeightsBuffer));
    //        glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * pAnimated->m_vWeights.size(), &pAnimated->m_vWeights[0], GL_STATIC_DRAW));
    //        glCall(glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, nullptr));
    //        glCall(glEnableVertexAttribArray(8));
    //    }
    //});
    
    setCompileInstanceDataFunc([this](IShaderConfigurator* pShaderConfig, size_t nIdx, ModelInstance* inst) {
        auto pConfig = static_cast<AnimatedModelShaderConfigurator*>(pShaderConfig);
        auto aniInst = static_cast<AnimatedModelInstance*>(inst);
        
		m_vJointTransforms.insert(m_vJointTransforms.end(), aniInst->vJointTransforms.begin(), aniInst->vJointTransforms.end());
		for (size_t i = 0; i < AnimatedModelShaderConfigurator::MAX_JOINTS - aniInst->vJointTransforms.size(); ++i)
			m_vJointTransforms.emplace_back(1.0);
        //pConfig->loadJointTransforms(nIdx, aniInst->vJointTransforms);
    });

	setPrepareInstanceDataFunc([this]() {
		m_vJointTransforms.clear();
	});

	setUpdateInstanceDataFunc([this](IShaderConfigurator* pShaderConfig, RenderObjectGL* pObj) {
		//RenderToolsGL::loadVBOData(pObj->bufferAt(4), m_vJointTransforms);
        auto pConfig = static_cast<AnimatedModelShaderConfigurator*>(pShaderConfig);
		
		pConfig->loadJointTransforms(m_vJointTransforms);
	});
}

//std::unique_ptr<IShaderConfigurator> AnimatedModelRendererGL::createShaderConfigurator() const {
//    return std::make_unique<AnimatedModelShaderConfigurator>();
//}

}
