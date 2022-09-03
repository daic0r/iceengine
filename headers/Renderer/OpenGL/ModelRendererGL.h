//
//  ModelRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 14.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ModelRendererGL_h
#define ModelRendererGL_h

#ifdef RENDERER_OPEN_GL

#include <Interfaces/IModelRenderer.h>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Renderer/OpenGL/TextureGL.h>
#include <Renderer/OpenGL/FramebufferObjectGL.h>
#include <memory>
#include <functional>

namespace Ice {

struct Model;
struct RenderEnvironment;
struct ModelInstance;
class IShaderProgram;
class IShaderConfigurator;
struct MeshComponent;
class TextureGL;
class IGraphicsSystem;
class ShadowShaderConfigurator;
class RenderMaterial;
class ModelShaderConfigurator;
class ShadowMapRendererGL;
class GraphicsSystemGL;

class ModelRendererGL : public IModelRenderer {
	std::vector<GLfloat> m_vWorldTransforms;
    IShaderConfigurator *m_pShaderConfig{nullptr};

    std::function<void(IShaderConfigurator*, size_t, ModelInstance*)> m_compileInstanceDataFunc;
	std::function<void(IShaderConfigurator*, RenderObjectGL*)> m_updateInstanceDataFunc;
	std::function<void()> m_prepareInstanceDataFunc;

protected:
	GraphicsSystemGL* m_pGraphicsSystem{ nullptr };
	ShadowMapRendererGL* m_pShadowMapRenderer{ nullptr };

    //virtual std::unique_ptr<IShaderConfigurator> createShaderConfigurator() const;
	//virtual std::unique_ptr<IShaderConfigurator> createShadowShaderConfigurator() const;
    void setCompileInstanceDataFunc(std::function<void(IShaderConfigurator*, size_t, ModelInstance*)> func) noexcept { m_compileInstanceDataFunc = func; }
	void setUpdateInstanceDataFunc(std::function<void(IShaderConfigurator*, RenderObjectGL*)> func) noexcept { m_updateInstanceDataFunc = func; }
	void setPrepareInstanceDataFunc(std::function<void()> func) noexcept { m_prepareInstanceDataFunc = func; }
    
public:
    ModelRendererGL() noexcept;
    void render(const RenderEnvironment&, const std::unordered_map<Model, std::vector<ModelInstance*>>&) noexcept override;
	//void renderShadowDepthTexture() const noexcept override;
	//GLuint getShadowDepthTextureId() const noexcept;
	//const glm::mat4& shadowProjectionViewMatrix() const noexcept { return m_shadowProjectionViewMatrix; }

private:
    virtual void prepareRendering(const RenderEnvironment&) noexcept;
	void _render(const RenderEnvironment&, const std::unordered_map<Model, std::vector<ModelInstance*>>&) noexcept;
    virtual void finishRendering() noexcept;

	virtual void prepareShader(RenderObjectGL* pModel, const RenderEnvironment& env) noexcept;
	virtual void unbindShader(RenderObjectGL* pModel) noexcept;
	virtual void setRenderMaterial(const RenderMaterial&, ModelShaderConfigurator* pConfig) noexcept;

};

}

#endif

#endif /* ModelRendererGL_h */
