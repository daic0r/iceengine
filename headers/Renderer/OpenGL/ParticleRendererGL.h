//
//  ParticleRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 10.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ParticleRendererGL_h
#define ParticleRendererGL_h

#ifdef RENDERER_OPEN_GL

#include "RenderObjectGL.h"
#include <vector>
#include <glm/mat4x4.hpp>
#include "../../Interfaces/IParticleRenderer.h"

namespace Ice {

class ParticleShaderConfigurator;
class IShaderProgram;
class Scene;
class Particle;
class Camera;
struct RenderEnvironment;
class ITexture;
class ParticleSystem;

class ParticleRendererGL : public IParticleRenderer {
    
    static constexpr int MAX_PARTICLES = 10000;
    static bool m_bInstanced;
    
    static const std::vector<GLfloat> m_vQuadVertices;
    std::unique_ptr<RenderObjectGL> m_pQuad;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    ParticleShaderConfigurator *m_pShaderConfig{ nullptr };
   
    std::vector<GLfloat> m_vRenderBuffer;
    GLuint m_VBO{ 0 };
    glm::mat4 m_projection;

    void prepareRendering(const RenderEnvironment&) noexcept;
	void finishRendering() noexcept;
public:
    ParticleRendererGL();
    ParticleRendererGL(const glm::mat4& proj);
    
    const glm::mat4& projectionMatrix() const noexcept override { return m_projection; }
    void setProjectionMatrix(const glm::mat4& proj) noexcept override;
    

    void render(const RenderEnvironment&, std::vector<ParticleSystem*>&) noexcept override;
};

}

#endif

#endif /* ParticleRendererGL_h */
