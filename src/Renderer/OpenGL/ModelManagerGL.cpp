#include <GL/glew.h>
#include <Renderer/OpenGL/ModelManagerGL.h>
#include <ShadersGL/ModelShaderConfigurator.h>
#include <System/Model.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <iostream>
#include <System/AnimatedModel.h>

namespace Ice {

void ModelManagerGL::registerModel(const Model* pModel) noexcept {
    if (m_mModels.find(pModel->pMesh) != m_mModels.end())
        return;

	//if (!m_pShadowProgram) {
	//	m_pShadowProgram = RenderToolsGL::createShaderProgram("Shadow", createShadowShaderConfigurator());
	//	m_pShadowShaderConfig = dynamic_cast<ShadowShaderConfigurator*>(m_pShadowProgram->configurator());
	//}
	//if (!m_pShadowTexDisplayerProgram) {
	//	m_pShadowTexDisplayerProgram = RenderToolsGL::createShaderProgram("TexDisplay", std::make_unique<TexDisplayShaderConfigurator>());
	//	m_pShadowTexDisplayerConfig = m_pShadowTexDisplayerProgram->configurator();
	//}
	//if (m_pGraphicsSystem == nullptr) {
	//	m_pGraphicsSystem = systemServices.getGraphicsSystem();
	//}

    auto glModel = RenderToolsGL::createRenderObjectAndLoadVBOsFromMesh(*pModel->pMesh);
	auto n = glModel->createEmptyVBO(10000 * 16);
	RenderToolsGL::addInstanceVertexAttribute(n, 3, 4, 16, 0);
	RenderToolsGL::addInstanceVertexAttribute(n, 4, 4, 16, 4);
	RenderToolsGL::addInstanceVertexAttribute(n, 5, 4, 16, 8);
	RenderToolsGL::addInstanceVertexAttribute(n, 6, 4, 16, 12);
	glCall(glEnableVertexAttribArray(3));
	glCall(glEnableVertexAttribArray(4));
	glCall(glEnableVertexAttribArray(5));
	glCall(glEnableVertexAttribArray(6));
	if (dynamic_cast<const AnimatedModel*>(pModel) != nullptr) {
        auto pAniModel = static_cast<const AnimatedModel*>(pModel);
        
        auto pAnimated = pAniModel->pAnimatedMesh;
        
        GLuint nJointIdBuffer{ 0 };
        if (pAnimated != nullptr && pAnimated->m_vJointIds.size() > 0) {
            glCall(glGenBuffers(1, &nJointIdBuffer));
            glCall(glBindBuffer(GL_ARRAY_BUFFER, nJointIdBuffer));
            glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::ivec4) * pAnimated->m_vJointIds.size(), &pAnimated->m_vJointIds[0], GL_STATIC_DRAW));
            glCall(glVertexAttribIPointer(7, 4, GL_INT, sizeof(GLint) * 4, nullptr));
            glCall(glEnableVertexAttribArray(7));
        }
        GLuint nJointWeightsBuffer{ 0 };
        if (pAnimated != nullptr && pAnimated->m_vWeights.size() > 0) {
            glCall(glGenBuffers(1, &nJointWeightsBuffer));
            glCall(glBindBuffer(GL_ARRAY_BUFFER, nJointWeightsBuffer));
            glCall(glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * pAnimated->m_vWeights.size(), &pAnimated->m_vWeights[0], GL_STATIC_DRAW));
            glCall(glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, nullptr));
            glCall(glEnableVertexAttribArray(8));
        }
	}
    //if (m_loadAdditionalVBOFunc)
    //    m_loadAdditionalVBOFunc(pModel, glModel.get());
    glCall(glBindVertexArray(0));
    
    auto shaderIter = m_mShaderPrograms.find(pModel->pMesh->shaderId());
    if (shaderIter == m_mShaderPrograms.end()) {
        auto p = m_mShaderPrograms.emplace(pModel->pMesh->shaderId(), RenderToolsGL::createShaderProgram(pModel->pMesh->shaderId(), RenderToolsGL::createShaderConfigurator(pModel->pMesh->shaderId())));
        shaderIter = p.first;
    }
    glModel->setShaderProgram(shaderIter->second.get());
    glModel->setShaderConfigurator(static_cast<ModelShaderConfigurator*>(shaderIter->second->configurator()));
    
    for (const auto& [strMaterialName, material] : pModel->pMaterials->materials()) {
        
        auto strTextureFile = material.textureFile();
        if (!strTextureFile.empty()) {
			auto [pTex, bInserted] = RenderToolsGL::registerTextureFromFile(strTextureFile, m_mTextures);
			if (bInserted) {
				auto pShaderConfig = glModel->shaderConfigurator();
                glModel->shaderProgram()->use();
                pShaderConfig->loadTextureUnit(0);
				//pShaderConfig->loadShadowDistance(100.0f);
				//pShaderConfig->loadShadowMargin(10.0f);
                glModel->shaderProgram()->unuse();
			}
            auto texIter = m_mTextures.find(strTextureFile);
            if (texIter == m_mTextures.end()) {
                auto [iter, bSuccess] = m_mTextures.emplace(strTextureFile, std::make_unique<TextureGL>());
                pTex = iter->second.get();
				pTex->setMinMagFilter(MinMagFilter::NEAREST);
                pTex->loadFromFile(strTextureFile);
                
                auto pShaderConfig = glModel->shaderConfigurator();
                glModel->shaderProgram()->use();
                pShaderConfig->loadTextureUnit(0);
                glModel->shaderProgram()->unuse();
            } else {
                pTex = texIter->second.get();
            }
            glModel->addTexture(pTex);
        }
    }
    
    m_mModels.emplace(pModel->pMesh, std::move(glModel));
}

void ModelManagerGL::unregisterModel(const Model *pModel) noexcept {
    m_mModels.erase(pModel->pMesh);
    for (const auto& [strMaterialName, material] : pModel->pMaterials->materials()) {
        auto strTextureFile = material.textureFile();
        m_mTextures.erase(strTextureFile);
    }
}

RenderObjectGL* ModelManagerGL::getModel(MeshComponent* pMesh)
{
	auto iter = m_mModels.find(pMesh);
    if (iter == m_mModels.end())
        throw std::runtime_error("Have no model for this mesh pointer");
	return iter->second.get();
}

}