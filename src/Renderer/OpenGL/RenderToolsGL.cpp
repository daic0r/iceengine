//
//  RenderToolsGL.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifdef RENDERER_OPEN_GL

#include <GL/glad.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <System/Config.h>
#include <iostream>
#include <Interfaces/IMasterRenderer.h>
#include <System/SystemServices.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <Interfaces/IShaderConfigurator.h>
#include <Interfaces/IShaderProgram.h>
#include <Interfaces/IGraphicsSystem.h>
#include <Components/MeshComponent.h>
#include <Components/AnimatedMeshComponent.h>
#include <cstring>
#include <ShadersGL/AnimatedModelShaderConfigurator.h>
#include <ShadersGL/ModelShaderConfigurator.h>

namespace Ice {

GLuint RenderToolsGL::createVAO() {
	GLuint nVao{ 0 };
	glCall(glGenVertexArrays(1, &nVao));
    glCall(glBindVertexArray(nVao));
    return nVao;
}

GLuint RenderToolsGL::createEmptyVBO(GLuint nVao, int nNumFloats, GLenum usage) {
    GLuint nVbo;
    glCall(glBindVertexArray(nVao));
    glCall(glGenBuffers(1, &nVbo));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, nVbo));
    glCall(glBufferData(GL_ARRAY_BUFFER, nNumFloats * sizeof(GLfloat), nullptr, usage));
    return nVbo;
}

void RenderToolsGL::addInstanceVertexAttribute(GLuint nVbo, int nAttribIndex, int nNumComponents, int nInstanceDataChunkLength, int nNumFloatsOffset, int nDivisor) {
    glCall(glBindBuffer(GL_ARRAY_BUFFER, nVbo));
    glCall(glVertexAttribPointer(nAttribIndex, nNumComponents, GL_FLOAT, GL_FALSE, nInstanceDataChunkLength * sizeof(GLfloat), reinterpret_cast<void*>(nNumFloatsOffset * sizeof(GLfloat))));
    glCall(glVertexAttribDivisor(nAttribIndex, nDivisor));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void RenderToolsGL::loadVBOData(GLuint nVbo, const std::vector<GLfloat>& vData) {
    glCall(glBindBuffer(GL_ARRAY_BUFFER, nVbo));
    //glCall(glBufferData(GL_ARRAY_BUFFER, vData.size() * sizeof(GLfloat), &vData[0], GL_DYNAMIC_DRAW));
    glCall(glBufferSubData(GL_ARRAY_BUFFER, 0, vData.size() * sizeof(GLfloat), &vData[0]));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

GLuint RenderToolsGL::loadFloatBuffer(GLenum nBufferType, const GLfloat* buffer, std::size_t nNumElements, GLenum nUsage) {
    GLuint nBuffer;
    glCall(glGenBuffers(1, &nBuffer));
    glCall(glBindBuffer( nBufferType, nBuffer));
    glCall(glBufferData( nBufferType, nNumElements * sizeof(GLfloat), &buffer[0], nUsage));
    
    return nBuffer;
}

template<typename VertexType>
RenderObjectGL RenderToolsGL::loadVerticesToVAO(const std::vector<VertexType>& buffer) {
    GLuint nVao;
    nVao = createVAO();
    
    RenderObjectGL obj{ nVao };
    GLuint nBuffer;
    glGenBuffers(1, &nBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, nBuffer);
    glBufferData(GL_ARRAY_BUFFER, buffer.size() * sizeof(VertexType), &buffer[0], GL_STATIC_DRAW);
    glCall(glVertexAttribPointer(0, VertexType::length(), GL_FLOAT, GL_FALSE, VertexType::length() * sizeof(GLfloat), nullptr));
    glCall(glBindBuffer( GL_ARRAY_BUFFER, 0));
    obj.addBuffer(nBuffer);
    
    return obj;
}

template RenderObjectGL RenderToolsGL::loadVerticesToVAO<glm::vec2>(const std::vector<glm::vec2>&);
template RenderObjectGL RenderToolsGL::loadVerticesToVAO<glm::vec3>(const std::vector<glm::vec3>&);

std::unique_ptr<RenderObjectGL> RenderToolsGL::createRenderObjectAndLoadVBOsFromMesh(const MeshComponent& mesh) {
    if (mesh.vertices().size() == 0)
        throw std::runtime_error("Vertex and index buffers must not be empty");
    
    auto pRenderObj = std::make_unique<RenderObjectGL>(createVAO());
    
    GLuint nVertexBuffer = RenderToolsGL::loadFloatBuffer(GL_ARRAY_BUFFER, reinterpret_cast<const GLfloat*>(&mesh.vertices()[0]), mesh.vertices().size() * 3,GL_STATIC_DRAW);
    glCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
    glCall(glEnableVertexAttribArray(0));

    GLuint nTexCoordBuffer{ 0 };
    if (mesh.texCoords().size() > 0) {
        nTexCoordBuffer = RenderToolsGL::loadFloatBuffer(GL_ARRAY_BUFFER, reinterpret_cast<const GLfloat*>(&mesh.texCoords()[0]), mesh.texCoords().size() * 2, GL_STATIC_DRAW);
        glCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, nullptr));
        glCall(glEnableVertexAttribArray(1));
    }
    GLuint nNormalBuffer{ 0 };
    if (mesh.normals().size() > 0) {
        nNormalBuffer = RenderToolsGL::loadFloatBuffer(GL_ARRAY_BUFFER, reinterpret_cast<const GLfloat*>(&mesh.normals()[0]), mesh.normals().size() * 3, GL_STATIC_DRAW);
        glCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, nullptr));
        glCall(glEnableVertexAttribArray(2));
    }

    GLuint nElementBuffer{ 0 };
    if (mesh.indices().size() > 0) {
        glCall(glGenBuffers(1, &nElementBuffer));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nElementBuffer));
        glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.indices().size(), &mesh.indices()[0], GL_STATIC_DRAW));
    } else
    if (mesh.materialIndices().size() > 0) {
        for (const auto& kvp : mesh.materialIndices()) {
            GLuint nIbo;
            glCall(glGenBuffers(1, &nIbo));
            glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, nIbo));
            glCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * kvp.second.size(), &kvp.second[0], GL_STATIC_DRAW));
            pRenderObj->addIndexBuffer(nIbo);
        }
    }
    
    pRenderObj->addBuffer(nVertexBuffer);
    pRenderObj->addBuffer(nTexCoordBuffer);
    pRenderObj->addBuffer(nNormalBuffer);
    if (nElementBuffer > 0)
        pRenderObj->addIndexBuffer(nElementBuffer);
    
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    
    return pRenderObj;
}

GLuint RenderToolsGL::createFramebuffer() {
    GLuint nFramebuffer;
    glCall(glGenFramebuffers(1, &nFramebuffer));
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, nFramebuffer));
    
    // We'll be rendering to attachment 0
    //glCall(glDrawBuffer(GL_COLOR_ATTACHMENT0));
    return nFramebuffer;
}

GLuint RenderToolsGL::createTextureFramebufferAttachment(int width, int height) {
    GLuint nTexture;
    glCall(glGenTextures(1, &nTexture));
    glCall(glBindTexture(GL_TEXTURE_2D, nTexture));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    glCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, nTexture, 0));
    glCall(glBindTexture(GL_TEXTURE_2D, 0));
    return nTexture;
}

GLuint RenderToolsGL::createDepthTextureFramebufferAttachment(int width, int height) {
    GLuint nTexture;
    glCall(glGenTextures(1, &nTexture));
    glCall(glBindTexture(GL_TEXTURE_2D, nTexture));
    glCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	glCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    glCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, nTexture, 0));
    glCall(glBindTexture(GL_TEXTURE_2D, 0));
    return nTexture;
}

GLuint RenderToolsGL::createDepthBufferFramebufferAttachment(int width, int height) {
    GLuint nDepthBuffer;
    glCall(glGenRenderbuffers(1, &nDepthBuffer));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, nDepthBuffer));
    glCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height));
    glCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, nDepthBuffer));
    glCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    return nDepthBuffer;
}

void RenderToolsGL::bindFramebuffer(GLuint nFramebuffer, int width, int height) {
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, nFramebuffer));
    glCall(glViewport(0, 0, width, height));
}

void RenderToolsGL::unbindCurrentFramebuffer(GLuint nLastBuffer, GLsizei nWidth, GLsizei nHeight) {
    glCall(glBindFramebuffer(GL_FRAMEBUFFER, nLastBuffer));
    glCall(glViewport(0, 0, nWidth > -1 ? nWidth : systemServices.getGraphicsSystem()->displayWidth(), nHeight > -1 ? nHeight : systemServices.getGraphicsSystem()->displayHeight()));
}

std::unique_ptr<GLubyte[]> RenderToolsGL::createUniformBuffer(GLuint nShaderProgram, const std::string& strName, const std::vector<const GLchar*>& vVarNames, std::vector<GLuint>& vOffsets, GLuint& nBuffer, GLint& nBufSize) {
    GLuint nBlockIndex = glCall(glGetUniformBlockIndex(nShaderProgram, strName.c_str()));
    glCall(glGetActiveUniformBlockiv(nShaderProgram, nBlockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &nBufSize));
    auto pBuf = std::make_unique<GLubyte[]>(nBufSize);

    GLint numBlocks;
    glGetProgramiv(nShaderProgram, GL_ACTIVE_UNIFORMS, &numBlocks);
    
    auto arIndices = new GLuint[vVarNames.size()];
    auto arOffsets = new GLint[vVarNames.size()];
    memset(arIndices, '\0', vVarNames.size() * sizeof(GLuint));
    memset(arOffsets, '\0', vVarNames.size() * sizeof(GLint));
    glCall(glGetUniformIndices(nShaderProgram, vVarNames.size(), &vVarNames[0], arIndices));
    glCall(glGetActiveUniformsiv(nShaderProgram, vVarNames.size(), arIndices, GL_UNIFORM_OFFSET, arOffsets));
    delete[] arIndices;
    
    vOffsets.clear();
    for (int i = 0; i < vVarNames.size(); ++i)
        vOffsets.emplace_back(arOffsets[i]);
    delete[] arOffsets;
    
    glCall(glGenBuffers(1, &nBuffer));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, nBuffer));
    glCall(glBufferData(GL_UNIFORM_BUFFER, nBufSize, nullptr, GL_DYNAMIC_DRAW));
    glCall(glBindBufferBase(GL_UNIFORM_BUFFER, nBlockIndex, nBuffer));
    glCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
    
    return pBuf;
}

std::unique_ptr<IShaderProgram> RenderToolsGL::createShaderProgram(const std::string& strId, std::unique_ptr<IShaderConfigurator> pConfig) noexcept {
//    std::string strFileVertexShader{ Config::BasePath };
//    strFileVertexShader.append("ShadersGL/");
//    strFileVertexShader.append(strId);
//    strFileVertexShader.append("VertexShader.glsl");
    std::string strFileVertexShader = systemServices.getConfig()->prependAssetPath("ShadersGL/" + strId + "VertexShader.glsl");

//    std::string strFileFragmentShader{ Config::BasePath };
//    strFileFragmentShader.append("ShadersGL/");
//    strFileFragmentShader.append(strId);
//    strFileFragmentShader.append("FragmentShader.glsl");
    std::string strFileFragmentShader = systemServices.getConfig()->prependAssetPath("ShadersGL/" + strId + "FragmentShader.glsl");

    auto pProgram = std::make_unique<ShaderProgramGL>(strFileVertexShader, strFileFragmentShader, std::move(pConfig));
    try {
        pProgram->load();
    }
    catch (const std::runtime_error& e) {
        std::cout << "Shader registration error: " << e.what() << std::endl;
        std::cout << "File is " << strFileVertexShader << " or " << strFileFragmentShader << '\n';
    }

    return pProgram;
}

std::pair<TextureGL*, bool> RenderToolsGL::registerTextureFromFile(const std::string& strFile, std::unordered_map<std::string, std::unique_ptr<TextureGL>>& mRegistry, MinMagFilter filter)
{
	TextureGL *pTex{ nullptr };
	bool bInserted;
    auto texIter = mRegistry.find(strFile);
    if (texIter == mRegistry.end()) {
        auto [iter, bSuccess] = mRegistry.emplace(strFile, std::make_unique<TextureGL>());
        pTex = iter->second.get();
		pTex->setMinMagFilter(MinMagFilter::NEAREST);
        pTex->loadFromFile(strFile);
		bInserted = true;
    } else {
        pTex = texIter->second.get();
		bInserted = false;
    }
	return std::make_pair(pTex, bInserted);
}

// #IDEA: Possibly move to separate factory class
std::unique_ptr<IShaderConfigurator> RenderToolsGL::createShaderConfigurator(std::string_view strShaderId) noexcept
{
    if (strShaderId == "Model") {
		return std::make_unique<ModelShaderConfigurator>();
	}
	else if (strShaderId == "AnimatedModel") {
		return std::make_unique<AnimatedModelShaderConfigurator>();
	}
	return std::unique_ptr<IShaderConfigurator>{};
}

}

#endif
