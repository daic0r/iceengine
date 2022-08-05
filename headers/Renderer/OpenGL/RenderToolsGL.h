//
//  RenderToolsGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef RenderToolsGL_h
#define RenderToolsGL_h

#ifdef RENDERER_OPEN_GL

#include <SDL2/SDL_opengl.h>
#include <vector>
#include "RenderObjectGL.h"
#include "TextureGL.h"

namespace Ice {

#include <vector>
#include <memory>

struct MeshComponent;
class IShaderProgram;
struct AnimatedMeshComponent;

class RenderToolsGL {
public:
    static GLuint createVAO();
    static GLuint createEmptyVBO(GLuint nVao, int nNumFloats, GLenum usage = GL_DYNAMIC_DRAW);
    static void addInstanceVertexAttribute(GLuint nVbo, int nAttribIndex, int nNumComponents, int nInstanceDataChunkLength, int nNumFloatsOffset, int nDivisor = 1);
    static void loadVBOData(GLuint nVbo, const std::vector<GLfloat>& vData);
    static GLuint loadFloatBuffer(GLuint nVAO, GLenum nBufferType, const std::vector<GLfloat>& buffer, GLenum nUsage);
    static RenderObjectGL loadVerticesToVAO(const std::vector<GLfloat>& buffer, int nDim);
    static std::unique_ptr<RenderObjectGL> createRenderObjectAndLoadVBOsFromMesh(const MeshComponent&);
    
    static GLuint createFramebuffer();
    static GLuint createTextureFramebufferAttachment(int width, int height);
    static GLuint createDepthTextureFramebufferAttachment(int width, int height);
    static GLuint createDepthBufferFramebufferAttachment(int width, int height);
    static void bindFramebuffer(GLuint nFramebuffer, int width, int height);
    static void unbindCurrentFramebuffer();
    
    static std::unique_ptr<GLubyte[]> createUniformBuffer(GLuint nShaderProgram, const std::string& strName, const std::vector<const GLchar*>& vVarNames, std::vector<GLuint>& vOffsets, GLuint& nBuffer, GLint& nBufSize);
    static std::unique_ptr<IShaderProgram> createShaderProgram(const std::string& strId, std::unique_ptr<IShaderConfigurator> pConfig) noexcept;
	static std::unique_ptr<IShaderConfigurator> createShaderConfigurator(std::string_view) noexcept;

	static std::pair<TextureGL*, bool> registerTextureFromFile(const std::string& strFile, std::unordered_map<std::string, std::unique_ptr<TextureGL>>& mRegistry, MinMagFilter filter = MinMagFilter::LINEAR);
};

}

#define glCall(x) \
x;\
{\
    GLenum err;\
    if((err = glGetError()) != GL_NO_ERROR)\
    {\
        std::cout << std::hex << "Error in call to " << #x << ":" << err << std::endl;\
    }\
}

#endif

#endif /* RenderToolsGL_h */
