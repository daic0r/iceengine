//
//  Renderer.cpp
//  IceCraft
//
//  Created by Matthias Gruen on 04.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#include <GL/glew.h>
#include <iostream>
#include <Renderer/OpenGL/GraphicsSystemGL.h>
#include <SDL2/SDL.h>
#include <System/Config.h>
#include <Renderer/OpenGL/RenderToolsGL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <IceEngine.h>
#include <System/Config.h>
#include <Renderer/OpenGL/ShadowMapRendererGL.h>


// testing
////

#include <System/SystemServices.h>
#include <Interfaces/ITextManager.h>

////

namespace Ice {

static const std::vector<GLfloat> vQuad{ 
    -1.0f, 1.0f,
    -1.0f, -1.0f,
    1.0f, 1.0f,
    1.0f, -1.0f
};

bool GraphicsSystemGL::m_bInstanced{ false };
static const char* arrow[] = {
    /* width height num_colors chars_per_pixel */
    "    32    32        3            1",
    /* colors */
    "X c #000000",
    ". c #ffffff",
    "  c None",
    /* pixels */
    "X                               ",
    "X.X                             ",
    "X...X                           ",
    "X.....X                         ",
    "X.......X                       ",
    "X.........X                     ",
    "X...........X                   ",
    "X.............X                 ",
    "X...............X               ",
    "X.................X             ",
    "X...................X           ",
    "X....................X          ",
    "X......XXXXXXXXXXXXXXXX         ",
    "X.....X                         ",
    "X....X                          ",
    "X...X                           ",
    "X..X                            ",
    "X.X                             ",
    "XX                              ",
    "X                               ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "                                ",
    "0,0"
};


GraphicsSystemGL::GraphicsSystemGL() {
    assert(!m_bInstanced);
    m_bInstanced = true;
}

bool initGL();

bool GraphicsSystemGL::init() {
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        std::cout << "SDL initialized." << std::endl;
    }
    else
    {
        return false;
    }
    
    int flags{ 0 };
    Config* pCfg = systemServices.getConfig();
    flags = pCfg->fullscreen() ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_SHOWN;
    flags |= SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;

    //Use OpenGL 3.1 core
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
    // Doublebuffering aktivieren
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);	// Cannot use 32, that makes shader program creation fail

    m_nWidth = pCfg->width();
    m_nHeight = pCfg->height();
    m_pWindow = SDL_CreateWindow(systemServices.getConfig()->gameName().data(), pCfg->windowPosX() == -1 ? SDL_WINDOWPOS_CENTERED : pCfg->windowPosX(), pCfg->windowPosY() == -1 ? SDL_WINDOWPOS_CENTERED : pCfg->windowPosY(), pCfg->width(), pCfg->height(), flags);
    
    if (m_pWindow == nullptr)
        return false;
    
    int nIndex = SDL_GetWindowDisplayIndex(window());
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(nIndex, &dm) != 0)
        return false;
    m_nScreenWidth = dm.w;
    m_nScreenHeight = dm.h;

    m_context = SDL_GL_CreateContext(m_pWindow);
    if (m_context == nullptr)
        return false;
    
    glewExperimental = GL_TRUE;
    auto init_res = glewInit();
    if(init_res != GLEW_OK)
    {
        std::cout << glewGetErrorString(glewInit()) << std::endl;
    }
    
    if( SDL_GL_SetSwapInterval( 1 ) < 0 )   // <-- vsync disabled
    {
        printf( "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError() );
    }
    SDL_StopTextInput(); // seems to be enabled by default, so disable it here so EventHandlingSystem will handle Key events correctly
    
    glEnable(GL_DEPTH_TEST);
    //Initialize clear color
    glClearColor( 0.0f, 1.f, 1.f, 1.f );
    glEnable(GL_MULTISAMPLE);
    
    updateProjectionMatrix();
    updateFramebuffer();
    
    m_pCursor = init_system_cursor(arrow);
    SDL_SetCursor(m_pCursor);
    SDL_StopTextInput();

    initGL();

    return true;  
}

void GraphicsSystemGL::beginRender() noexcept {
    // Bind FBO to render to
    //m_fbo.bind();
    glClear( GL_COLOR_BUFFER_BIT);
    glClear (GL_DEPTH_BUFFER_BIT);
}

void GraphicsSystemGL::endRender() noexcept {
    // Unbind the FBO, thereby enabling rendering to the actual screen
    /*
    m_fbo.unbind();
    glClear (GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    // Bind quad VAO
    glBindVertexArray(m_quad.vao());
    // Activate the shader
    m_quadShader.use();
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_fbo.textureAttachmentId());
    // Render the quad
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // Cleanup
    m_quadShader.unuse();
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    */
    SDL_GL_SwapWindow( m_pWindow );
}

//bool GraphicsSystemGL::render(float fDeltaTime, Camera* pCamera) {
//
//    [[maybe_unused]] static int nFPS = 0;
//    [[maybe_unused]] static int nLastTicks = SDL_GetTicks();
//    [[maybe_unused]] static float fFPS = 0.0f;
//
//    RenderEnvironment env;
//    //env.lights.emplace_back(&world.sun());
//    env.fDeltaTime = fDeltaTime;
//    //env.lights = pScene->lights();
//    env.projectionMatrix = m_projectionMatrix;
//    env.viewMatrix = pCamera->matrix();
//    env.pCamera = pCamera;
//    Frustum frust{ *env.pCamera, .1f, 500.0f, glm::radians(fov()), (float)displayWidth() / (float) displayHeight() };
//    env.pFrustum = &frust;
    
    
// ==========================
//    glCall(glEnable(GL_CLIP_DISTANCE0));
    
//    m_pWaterFramebuffers->bindRefractionFramebuffer();
//    env.clipPlane = glm::vec4(0.0f, -1.0f, 0.0f, Config::WATER_LEVEL);
//    renderScene(env, pScene);
//    RenderToolsGL::unbindCurrentFramebuffer();
    
//    m_pWaterFramebuffers->bindReflectionFramebuffer();
//    env.clipPlane = glm::vec4(0.0f, 1.0f, 0.0f, -Config::WATER_LEVEL + 1);
//    auto reflectionCam = Camera{ *pCamera };
//    auto fDistance = 2.0f * (reflectionCam.position().y - Config::WATER_LEVEL);
//    reflectionCam.setPosition(glm::vec3{ reflectionCam.position().x, reflectionCam.position().y - fDistance, reflectionCam.position().z });
//    reflectionCam.setYawAndPitch(reflectionCam.yaw(), -reflectionCam.pitch());
//    env.pCamera = &reflectionCam;
//    env.viewMatrix = reflectionCam.matrix();
//    renderScene(env, pScene);
//    RenderToolsGL::unbindCurrentFramebuffer();
//
//    glCall(glDisable(GL_CLIP_DISTANCE0));
//    env.clipPlane = glm::vec4(0.0f, -1.0f, 0.0f, 1000000.0f);
//    env.viewMatrix = Controller::instance().camera().matrix();
//    env.pCamera = &Controller::instance().camera();
// -->> renderScene(env);
//    ParticleManager::instance().render(env);
//
//    env.pMiscData = static_cast<void*>(m_pWaterFramebuffers.get());
//    m_pWaterRenderer->render(env, pScene->waterTiles());
    
    //frust.render();

//    auto playerPos = Controller::instance().player().position();
//    ++nFPS;
//    int nCurrentTicks = SDL_GetTicks();
//    if (nCurrentTicks - nLastTicks >= 1000) {
//        int nDiff = nCurrentTicks - nLastTicks;
//        fFPS = ((float) nFPS / nDiff) * 1000.0f;
//        nFPS = 0;
//        nLastTicks = nCurrentTicks;
//    }
//    std::stringstream ss;
//    ss << "FPS: " << fFPS << " Player at: (" << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << ")" << std::endl;
//
//    textRenderer.renderText(ss.str(), 5, 5);
    
//---> textManager->render();
//
//
//    //std::cout << ss.str() << std::endl;
//    //ParticleManager::instance().render(env);
//
//    //Update screen
//---> SDL_GL_SwapWindow( m_pWindow );

//    return true;
//}

void GraphicsSystemGL::shutDown() {
    std::cout << "Shutting down SDL" << std::endl;
	SDL_FreeCursor(m_pCursor);
    SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(window());
#ifndef WINDOWS_SYSTEM
	SDL_Quit();
#endif
}

GraphicsSystemGL::~GraphicsSystemGL() {
	shutDown();
}

SDL_Window* GraphicsSystemGL::window() const noexcept {
    return m_pWindow;
}

SDL_GLContext GraphicsSystemGL::context() const noexcept {
    return m_context;
}

void GraphicsSystemGL::initGL() {
    /*
    m_quad = RenderToolsGL::loadVerticesToVAO(vQuad, 2);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    m_quadShader.fromSource(ShaderProgramGL::canvasVertexShader(), ShaderProgramGL::canvasFragmentShader());
    m_quadShader.use();
    m_texUniformId = m_quadShader.getUniformLocation("tex");
    m_quadShader.loadInt(m_texUniformId, 0);
    m_quadShader.unuse();
    */
}

//void GraphicsSystemGL::registerModel(Model *pModel) {
//    //m_mModels.emplace(pModel, std::make_unique<GLModel>(pModel));
//    //m_pModelRenderer->registerModel(pModel);
//}
//
//void GraphicsSystemGL::renderSkybox(const RenderEnvironment& env) {
//    GLint OldDepthFuncMode;
//    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
//
//    glCall(glDisable(GL_DEPTH_TEST));
//    glCall(glDepthFunc(GL_LEQUAL));
//    m_pSkybox->prepareRendering(env);
//    m_pSkybox->render();
//    m_pSkybox->finishRendering();
//    glCall(glEnable(GL_DEPTH_TEST));
//    glCall(glDepthFunc(OldDepthFuncMode));
//}
//
//void GraphicsSystemGL::renderScene(const RenderEnvironment& env) {
//    glCall(glClear( GL_COLOR_BUFFER_BIT));
//    glCall(glClear (GL_DEPTH_BUFFER_BIT));
//    renderSkybox(env);
//    m_pTerrainRenderingSystem->setRenderEnvironment(env);
//    m_pTerrainRenderingSystem->update(0.0f);
//    //m_pTerrainRenderer->render(env, pScene->terrains());
//    //m_pModelRenderer->render(env, pScene->modelInstances());
//    m_pObjectRenderingSystem->render(env);
//}
//
//void GraphicsSystemGL::initSkybox() {
//    m_pSkybox = std::make_unique<SkyboxGLModel>();
//    m_pSkybox->loadCubeMap();
//    m_pSkybox->load(true, true, false, false, [] (GLModel& glModel) {
//        glModel.setShader(RenderToolsGL::createShaderProgram("Skybox", std::make_unique<SkyboxShaderConfigurator>()));
//        //setShader(shaderManager->getShader(model()->shaderId()));
//        glModel.setShaderConfigurator(dynamic_cast<Shader3dConfigurator*>(glModel.shader()->configurator()));
//    });
//}

void GraphicsSystemGL::resizeWindow(int width, int height) noexcept {
    glCall(glViewport(0, 0, width, height));
    m_nWidth = width;
    m_nHeight = height;
    updateProjectionMatrix();
    updateFramebuffer();
}

void GraphicsSystemGL::updateProjectionMatrix() noexcept {
	// Use 1.0f for z_near; if 0.1f is used, this will lead to z-fighting and artifacts,
	// if the default depth buffer precision of 16-bits is used. With 24 bits, even 0.1f seems
	// to work alright; however, despite using 24 bits I'll use 1.0f just to be sure
    m_projectionMatrix = glm::perspective(glm::radians(fov()), (float)m_nWidth/(float)m_nHeight, distNearPlane(), distFarPlane());
}

bool GraphicsSystemGL::updateFramebuffer() noexcept {
    /*
    m_fbo = FramebufferObjectGL( displayWidth(), displayHeight() );
    m_fbo.create();
    assert(m_fbo.createTextureAttachment());
    assert(m_fbo.createDepthAttachment());
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
    return true;
}

void GraphicsSystemGL::setFov(float fFov) noexcept {
    m_fFoV = fFov;
    updateProjectionMatrix();
}

float GraphicsSystemGL::getNormalizedPixelsHorizontal(int n) const noexcept {
    return static_cast<float>(n) * getHorizontalPixelScale();
}

float GraphicsSystemGL::getNormalizedPixelsVertical(int n) const noexcept {
    return static_cast<float>(n) * getVerticalPixelScale();
}

float GraphicsSystemGL::getHorizontalPixelScale() const noexcept {
    return 2.0f / displayWidth();
}

float GraphicsSystemGL::getVerticalPixelScale() const noexcept {
    return 2.0f / displayHeight();
}

SDL_Cursor* GraphicsSystemGL::init_system_cursor(const char* image[])
{
    int i, row, col;
    Uint8 data[4 * 32];
    Uint8 mask[4 * 32];
    int hot_x, hot_y;

    i = -1;
    for (row = 0; row < 32; ++row) {
        for (col = 0; col < 32; ++col) {
            if (col % 8) {
                data[i] <<= 1;
                mask[i] <<= 1;
            }
            else {
                ++i;
                data[i] = mask[i] = 0;
            }
            switch (image[4 + row][col]) {
            case 'X':
                data[i] |= 0x01;
                mask[i] |= 0x01;
                break;
            case '.':
                mask[i] |= 0x01;
                break;
            case ' ':
                break;
            }
        }
    }
    std::sscanf(image[4 + row], "%d,%d", &hot_x, &hot_y);
    return SDL_CreateCursor(data, mask, 32, 32, hot_x, hot_y);
}

}
