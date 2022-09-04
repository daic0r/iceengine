//
//  ModelRendererGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 14.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <Renderer/OpenGL/RenderToolsGL.h>

#include <iostream>
#include <vector>

#include <Renderer/OpenGL/ModelRendererGL.h>
#include <Renderer/RenderEnvironment.h>
#include <Renderer/Frustum.h>
#include <System/ModelInstance.h>
#include <System/Scene.h>
#include <System/Tools.h>
#include <System/Model.h>

#include <ShadersGL/Shader3dConfigurator.h>

#include <System/SystemServices.h>
#include <Interfaces/IShaderManager.h>
#include <Interfaces/IShaderProgram.h>
#include <ShadersGL/ModelShaderConfigurator.h>
#include <ShadersGL/ShadowShaderConfigurator.h>
#include <ShadersGL/TexDisplayShaderConfigurator.h>
#include <Components/MeshComponent.h>
#include <Components/RenderMaterialsComponent.h>
#include <Components/TransformComponent.h>
#include <Components/ModelReferenceComponent.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <Utils/ScopedTimeMeasurement.h>
#include <glm/gtc/type_ptr.hpp>
#include <Interfaces/IGraphicsSystem.h>
#include <Interfaces/IModelManager.h>
#include <Renderer/OpenGL/ShadowMapRendererGL.h>
#include <Renderer/OpenGL/GraphicsSystemGL.h>

namespace Ice {

ModelRendererGL::ModelRendererGL() noexcept {
	m_pGraphicsSystem = dynamic_cast<GraphicsSystemGL*>(systemServices.getGraphicsSystem());
}

void ModelRendererGL::prepareRendering(const RenderEnvironment&) noexcept {

	glCall(glEnable(GL_CULL_FACE));
	glCall(glEnable(GL_BLEND));
	glCall(glEnable(GL_DEPTH_TEST));
	glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

void ModelRendererGL::render(const RenderEnvironment& env, const std::unordered_map<Model, std::vector<ModelInstance*>>& instances) noexcept {

    prepareRendering(env);
	_render(env, instances);
    finishRendering(env);
}

void ModelRendererGL::finishRendering(const RenderEnvironment& env) noexcept {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glCall(glBindVertexArray(0));

	glCall(glDisable(GL_CULL_FACE));
	glCall(glDisable(GL_BLEND));
	glCall(glDisable(GL_DEPTH_TEST));
	if (env.fWaterLevel.has_value())
		glCall(glDisable(GL_CLIP_DISTANCE0));
}

void ModelRendererGL::prepareShader(RenderObjectGL* pModel, const RenderEnvironment& env) noexcept {
	pModel->shaderProgram()->use();
	pModel->shaderConfigurator()->loadUniforms(env);
	if (env.fWaterLevel.has_value()) {
		pModel->shaderConfigurator()->loadWaterLevelAndClipPlaneY(*env.fWaterLevel, static_cast<int>(env.clipMode));
		glEnable(GL_CLIP_DISTANCE0);
	}        
	glCall(glActiveTexture(GL_TEXTURE1));
	auto nId = dynamic_cast<ShadowMapRendererGL*>(systemServices.getShadowMapRenderer())->getShadowDepthTextureId();
	glCall(glBindTexture(GL_TEXTURE_2D, nId));

	if (env.pSun) {
		//auto pShaderConfig = static_cast<ModelShaderConfigurator*>(pModel->shaderConfigurator());
		//pShaderConfig->loadSunPosition(env.pSun->position());
		//pShaderConfig->loadSunColor(env.pSun->color());
		m_pGraphicsSystem->commonUBO().bind();
		m_pGraphicsSystem->commonUBO().loadSunPosition(env.pSun->position());
		m_pGraphicsSystem->commonUBO().loadSunColor(env.pSun->color());
		m_pGraphicsSystem->commonUBO().loadSunAmbient(env.pSun->ambient());
		m_pGraphicsSystem->commonUBO().unbind();
		//pShaderConfig->loadSunAmbient(env.pSun->ambient());
	}
}

void ModelRendererGL::unbindShader(RenderObjectGL* pModel, const RenderEnvironment& env) noexcept {
	if (env.fWaterLevel.has_value()) {
		pModel->shaderConfigurator()->loadWaterLevelAndClipPlaneY(0.0f, 0);
		glDisable(GL_CLIP_DISTANCE0);
	}        
	pModel->shaderProgram()->unuse();
}

void ModelRendererGL::setRenderMaterial(const RenderMaterial& mat, ModelShaderConfigurator* pConfig) noexcept {
	pConfig->setRenderMaterial(mat);
}

//GLuint ModelRendererGL::getShadowDepthTextureId() const noexcept {
//	return m_shadowFbo.depthTextureAttachmentId();
//}

//void ModelRendererGL::renderShadowDepthTexture() const noexcept {
//	glCall(glBindVertexArray(m_shadowTexObj.vao()));
//	m_pShadowTexDisplayerProgram->use();
//	glCall(glActiveTexture(GL_TEXTURE0));
//	glCall(glBindTexture(GL_TEXTURE_2D, m_shadowFbo.depthTextureAttachmentId()));
//	glCall(glDisable(GL_DEPTH_TEST));
//	glCall(glDisable(GL_BLEND));
//	glCall(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
//	glCall(glEnable(GL_DEPTH_TEST));
//	glCall(glEnable(GL_BLEND));
//	glCall(glBindTexture(GL_TEXTURE_2D, 0));
//	m_pShadowTexDisplayerProgram->unuse();
//	glCall(glBindVertexArray(0));
//}

void ModelRendererGL::_render(const RenderEnvironment& env, const std::unordered_map<Model, std::vector<ModelInstance*>>& instances) noexcept {
    [[maybe_unused]] int nCount{ 0 };

    // Render model instances, opaque first
	for (const auto& kvp : instances) {
		if (kvp.second.empty())
			continue;

		auto pModel = systemServices.getModelManager()->getModel(kvp.first.pMesh);
		//if (iter == m_mModels.end()) {
		//	registerModel(kvp.first);
		//	iter = m_mModels.find(kvp.first->pMesh);
		//}

		glCall(glBindVertexArray(pModel->vao()));

		auto pShaderConfig = static_cast<ModelShaderConfigurator*>(pModel->shaderConfigurator());

		prepareShader(pModel, env);

		if (m_prepareInstanceDataFunc)
			m_prepareInstanceDataFunc();
		// Render all instances of this model
		size_t nInsCount = 0;
		glBindBuffer(GL_ARRAY_BUFFER, pModel->bufferAt(3));
		glm::mat4* arMatrices = static_cast<glm::mat4*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		for (auto pInst : kvp.second) {
			arMatrices[nInsCount++] = pInst->pTransform->m_transform;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		if (m_updateInstanceDataFunc)
			m_updateInstanceDataFunc(pModel->shaderConfigurator(), pModel);

		int nBufferIndex = 0;
		for (const auto& [strMaterial, vIndices] : kvp.first.pMesh->materialIndices()) {
			//dynamic_cast<Shader3dConfigurator*>(pModel->shaderConfigurator())->loadDiffuseColor(kvp.first.getMaterial(kvp2.first).diffuse());
			const auto& material = kvp.first.pMaterials->getMaterial(strMaterial);
			
			setRenderMaterial(material, pShaderConfig);

			auto pTex = !material.textureFile().empty() ? pModel->textureAt(nBufferIndex) : nullptr;
			if (pTex != nullptr)
				pTex->bind(0);

			//m_vWorldTransforms.clear();
		/*
			for (auto pInst : kvp.second) {

				auto ptr = glm::value_ptr(pInst->pTransform->m_transform);
				m_vWorldTransforms.insert(m_vWorldTransforms.end(), ptr, ptr + 16);

				if (m_compileInstanceDataFunc)
					m_compileInstanceDataFunc(pModel->shaderConfigurator(), nInsCount, pInst);

				++nInsCount;
				//glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pModel->indexBufferAt(nBufferIndex)));
				//glCall(glDrawElements(GL_TRIANGLES, vIndices.size(), GL_UNSIGNED_INT, nullptr));
				
			}
			RenderToolsGL::loadVBOData(pModel->bufferAt(3), m_vWorldTransforms);
			*/
			glDrawElementsInstanced(GL_TRIANGLES, vIndices.size(), GL_UNSIGNED_INT, &vIndices[0], kvp.second.size());
			++nBufferIndex;
			if (pTex != nullptr)
				pTex->unbind();

			//pModel->render();

//            if (pInst->model()->verticesTransparent().size() > 0)
//                vTransparentModelsToRender.emplace_back(pInst);

//            std::cout << "Rendering model " << nCount++ << std::endl;
		}

		unbindShader(pModel, env);
    }
//    auto vpMat = env.projectionMatrix * env.viewMatrix;
//
//    // Sort transparent meshes by z-value
//    std::sort(vTransparentModelsToRender.begin(), vTransparentModelsToRender.end(), [&vpMat](ModelInstance* a, ModelInstance* b) {
//        auto tmpVertA = glm::vec3{ a->model()->minBounds()[0], ainmodel()->minBounds()[1], a->model()->minBounds()[2] };
//        auto tmpVertB = glm::vec3{ b->model()->minBounds()[0], b->model()->minBounds()[1], b->model()->minBounds()[2] };
//
//        tmpVertA = Tools::transformWithPerspectiveDivision(tmpVertA, vpMat * a->worldTransform());
//        tmpVertB = Tools::transformWithPerspectiveDivision(tmpVertB, vpMat * a->worldTransform());
//
//        return tmpVertA.z > tmpVertB.z; // render objects furthest away first
//    });
//
//
//    // Render transparent models
//    for (auto pInst : vTransparentModelsToRender) {
//        auto &pModel = m_mModels.at(pInst->model());
//        pModel->setTransparentMode(true);
//        pModel->prepareRendering(env);
//        pModel->shaderConfigurator()->loadModelMatrix(pInst->worldTransform());
//        pModel->render();
//        pModel->finishRendering();
//        pModel->setTransparentMode(false);
//    }
}

//std::unique_ptr<IShaderConfigurator> ModelRendererGL::createShaderConfigurator() const {
//    return std::make_unique<ModelShaderConfigurator>();
//}
//
//std::unique_ptr<IShaderConfigurator> ModelRendererGL::createShadowShaderConfigurator() const {
//	return std::make_unique<ShadowShaderConfigurator>();
//}

}
