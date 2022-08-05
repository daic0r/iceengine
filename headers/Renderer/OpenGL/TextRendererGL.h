//
//  TextRendererGL.h
//  IceCraft
//
//  Created by Matthias Gruen on 17.09.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef TextRendererGL_h
#define TextRendererGL_h

#ifdef RENDERER_OPEN_GL

#include <Interfaces/ITextRenderer.h>
#include "RenderObjectGL.h"
#include "RenderToolsGL.h"
#include <Interfaces/IShaderProgram.h>
#include <System/BaseTexture.h>
#include <Renderer/OpenGL/TextureGL.h>

namespace Ice {

namespace GUI {
    class TextLabel;
}

class TextShaderConfigurator;

class TextRendererGL : public ITextRenderer {
    std::unordered_map<TextureGL*, std::unique_ptr<RenderObjectGL>> m_mRenderObjects;
    std::unordered_map<std::string, std::unique_ptr<TextureGL>> m_mTextures;
    std::unique_ptr<IShaderProgram> m_pShaderProgram{ nullptr };
    TextShaderConfigurator* m_pShaderConfig{ nullptr };
    
    std::unordered_map<GUI::TextLabel*, std::unique_ptr<RenderObjectGL>> m_mLabelBuffer;
public:
    TextRendererGL();
    void render(GUI::TextLabel*) noexcept override;
    BaseTexture* registerFontTexture(const std::string& strName, const std::string& strTextureFile) override;
    
private:
    void finishRendering(TextureGL*) noexcept;
    
    TextureGL* prepareRendering(GUI::TextLabel*) noexcept;
};

}

#endif

#endif /* TextRendererGL_h */
