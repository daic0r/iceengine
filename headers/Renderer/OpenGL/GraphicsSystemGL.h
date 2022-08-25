//
//  Renderer.h
//  CovidSimulator
//
//  Created by Matthias Gruen on 04.08.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef SDLRenderer_h
#define SDLRenderer_h

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>
#include <optional>
#include <Renderer/OpenGL/CommonMatrixUBO_GL.h>
#include <Interfaces/IGraphicsSystem.h>
#include <ShadersGL/ShaderProgramGL.h>
#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <Renderer/OpenGL/RenderObjectGL.h>

struct SDL_Window;

namespace Ice {

class FrustumShaderConfigurator;
class ShaderProgramGL;
class Scene;
struct Model;
struct RenderEnvironment;
class IModelRenderer;
class Camera;
class IWaterRenderer;
class WaterFramebuffersGL;
class SkyboxGLModel;
class ITerrainRenderer;
class TerrainRenderingSystem;
class ObjectRenderingSystem;

class GraphicsSystemGL : public IGraphicsSystem {
private:
    static bool m_bInstanced;

public:
    GraphicsSystemGL();
   
    bool init() override;
    void beginRender() noexcept override;
    void endRender() noexcept override;
    void resizeWindow(int width, int height) noexcept override;
    const glm::mat4& projectionMatrix() const noexcept override { return m_projectionMatrix; }
    int displayWidth() const noexcept override { return m_nWidth; }
    int displayHeight() const noexcept override { return m_nHeight; }
    int screenWidth() const noexcept override { return m_nScreenWidth; }
    int screenHeight() const noexcept override { return m_nScreenHeight; }
    float fov() const noexcept override { return m_fFoV; }
    void setFov(float fFov) noexcept override;
    float getNormalizedPixelsHorizontal(int) const noexcept override;
    float getNormalizedPixelsVertical(int) const noexcept override;
    float getHorizontalPixelScale() const noexcept override;
    float getVerticalPixelScale() const noexcept override;
	float distNearPlane() const noexcept override { return 1.0f; }
	float distFarPlane() const noexcept override { return 500.0f; }
	float aspectRatio() const noexcept override { return static_cast<float>(m_nWidth) / static_cast<float>(m_nHeight); }
	void shutDown();
    ~GraphicsSystemGL();

    SDL_Window* window() const noexcept;
    SDL_GLContext context() const noexcept;
    auto& commonUBO() noexcept {
        static CommonMatrixUBO_GL _common{};
        return _common;
    }

private:
    static SDL_Cursor* init_system_cursor(const char* image[]);
    void updateProjectionMatrix() noexcept;
    bool updateFramebuffer() noexcept;
    void initGL();

    glm::mat4 m_projectionMatrix;
    float m_fFoV{ 45.0f };
    int m_nWidth, m_nHeight;
    int m_nScreenWidth{ 0 }, m_nScreenHeight{ 0 };
    
    SDL_Cursor* m_pCursor{ nullptr };
    SDL_Window* m_pWindow{ nullptr };
    SDL_GLContext m_context{ nullptr };

    RenderObjectGL m_quad;
    FramebufferObjectGL m_fbo;
    ShaderProgramGL m_quadShader;
    GLuint m_texUniformId{};
};

}

#endif /* SDLRenderer_h */
