//
//  WidgetRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 13.12.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef WidgetRendererGL_h
#define WidgetRendererGL_h

#include <SDL2/SDL_opengl.h>
#include <System/Extents.h>
#include <System/Types.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <Renderer/OpenGL/RenderObjectGL.h>
#include <Interfaces/IWidgetRenderer.h>
#include <Renderer/OpenGL/TextRendererGL.h>
#include <chrono>
#include <GUI/Widget.h>
#include <Renderer/OpenGL/FramebufferObjectGL.h>

namespace Ice {

class RenderObjectGL;
class WidgetShaderConfigurator;
class IShaderProgram;
struct RenderEnvironment;
class TextShaderConfigurator;
class BaseTexture;
class ModelRenderer;

namespace GUI {
    class Widget;
    class TextLabel;
    class ImageView;
    class ModelPreview;
    class ScrollWidget;
}

class WidgetRendererGL : public IWidgetRenderer {
    static inline bool m_bInstanced{ false };
    static inline const std::vector<glm::vec2> m_vQuadVertices{
        glm::vec2{ 0.0f, 0.0f },
        glm::vec2{ 0.0f, 1.0f },
        glm::vec2{ 1.0f, 0.0f },
        glm::vec2{ 1.0f, 1.0f }
    };
    
    std::unique_ptr<RenderObjectGL> m_pQuad;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    WidgetShaderConfigurator *m_pShaderConfig{ nullptr };
    
    ITextRenderer* m_pTextLabelRenderer;
    IModelRenderer *m_pModelRenderer;
    
    struct sModelPreviewInfo {
        FramebufferObjectGL fbo{};
        GLuint nTexId{ 0 };
        glm::mat4 projMatrix;
    };
    std::unordered_map<const GUI::ModelPreview*, sModelPreviewInfo> m_mModelPreviewInfo;
    
public:
    WidgetRendererGL();

    void render(const RenderEnvironment&, const std::vector<GUI::Widget*>&) noexcept override;

    void renderWidget(const GUI::Widget*) override;
    void renderWidget(const GUI::Button*) override;
    void renderWidget(GUI::TextLabel*) override;
    void renderWidget(const GUI::ImageView*) override;
    void renderWidget(const GUI::ModelPreview*) override;
    void renderWidget(const GUI::ScrollWidget*) override;

private:
    void _renderWidget(const GUI::Widget*, const std::function<void()>& additionalSetup = nullptr, const std::function<void()>& additionalDeinit = nullptr);
};

}

#endif /* WidgetRendererGL_h */
